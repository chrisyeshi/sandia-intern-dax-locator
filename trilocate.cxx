#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/cont/Scheduler.h>
#include <dax/exec/WorkletMapField.h>
#include <dax/cont/DeviceAdapter.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>
#include <dax/math/Compare.h>
#include <dax/math/Precision.h>

#include "tests/TriangleMesh2DGenerator.h"
#include "CountOverlappingBuckets.h"
#include "Offset2CountFunctor.h"
#include "Coarse2ImplicitFunctor.h"

using namespace dax::cont;

typedef dax::cont::internal::DeviceAdapterAlgorithm<DAX_DEFAULT_DEVICE_ADAPTER_TAG>
    Algorithm;

// This worklet parallelize on each cell,
// find what buckets each cell overlaps,
// then output cell ids to CellIds array,
// and bucket ids to BucketIds array,
// user needs to set CellIds and BucketIds prior to invoking the worklet.
class BinTriangles : public dax::exec::internal::WorkletBase
{
public:
    // inputs
    ArrayHandle<dax::Vector2>::PortalConstExecution PointsPortal;
    ArrayHandle<int>::PortalConstExecution ConnectionsPortal;
    ArrayHandle<int>::PortalConstExecution ScanBucketCounts;
    dax::Extent3 GridExtent;
    dax::Id3 GridDivisions;

    // the output arrays
    ArrayHandle<dax::Id>::PortalExecution CellIds;
    ArrayHandle<dax::Id>::PortalExecution BucketIds;

    // constructor
    BinTriangles(ArrayHandle<dax::Vector2>::PortalConstExecution pointsPortal,
                 ArrayHandle<int>::PortalConstExecution connectionsPortal,
                 ArrayHandle<int>::PortalConstExecution overlappingBucketCounts,
                 dax::Extent3 gridExtent,
                 dax::Id3 gridDivisions,
                 ArrayHandle<dax::Id>::PortalExecution cellIds,
                 ArrayHandle<dax::Id>::PortalExecution bucketIds)
               : PointsPortal(pointsPortal),
                 ConnectionsPortal(connectionsPortal),
                 ScanBucketCounts(overlappingBucketCounts),
                 GridExtent(gridExtent),
                 GridDivisions(gridDivisions),
                 CellIds(cellIds),
                 BucketIds(bucketIds)
    {}

    DAX_EXEC_EXPORT
    void operator()(dax::Id index) const
    {
        // the index here is the index of the triangles
        // vertices of this triangle
        const int verticesPerTriangle = 3;
        dax::Id vertexIds[verticesPerTriangle];
        dax::Vector2 vertices[verticesPerTriangle];
        for (int i = 0; i < verticesPerTriangle; ++i)
        {
            vertexIds[i] = this->ConnectionsPortal
                .Get(index * verticesPerTriangle + i);
            vertices[i] = this->PointsPortal.Get(vertexIds[i]);
        }
        // bounding box of the triangle
        dax::Vector2 lowerLeft = vertices[0];
        dax::Vector2 upperRight = vertices[0];
        for (int i = 0; i < verticesPerTriangle; ++i)
        {
            lowerLeft = dax::math::Min<dax::Vector2>(lowerLeft, vertices[i]);
            upperRight = dax::math::Max<dax::Vector2>(upperRight, vertices[i]);
        }
        // using the bounding box to estimate the number of overlapping buckets
        // find the bucket that the lower left cornor of the bounding box is in
        // find the bucket that the upper right cornor of the bounding box is in
        // they both define the quad of buckets the triangle overlaps
        dax::Id3 lowerLeftBucket = binPoint(lowerLeft);
        dax::Id3 upperRightBucket = binPoint(upperRight);
        // draw a quad using these two bucket ids
        int xRange = upperRightBucket[0] - lowerLeftBucket[0] + 1;
        int yRange = upperRightBucket[1] - lowerLeftBucket[1] + 1;
        for (int y = lowerLeftBucket[1]; y <= upperRightBucket[1]; ++y)
            for (int x = lowerLeftBucket[0]; x <=upperRightBucket[0]; ++x)
            {
                // put the cell id and bucket id into their appropriate positions
                dax::Id triangleId = index;
                dax::Id bucketId = x + y * this->GridDivisions[0];
                dax::Id start = this->ScanBucketCounts.Get(triangleId);
                dax::Id offset = (x - lowerLeftBucket[0])
                               + (y - lowerLeftBucket[1]) * xRange;
                this->CellIds.Set(start + offset, triangleId);
                this->BucketIds.Set(start + offset, bucketId);
            }
    }

    DAX_EXEC_EXPORT
    dax::Id3 binPoint(const dax::Vector2& point) const
    {
        // compute the point coordinate within the grid
        dax::Vector2 coord(point[0] - this->GridExtent.Min[0],
                           point[1] - this->GridExtent.Min[1]);
        // ranges of the whole grid
        dax::Vector2 range(this->GridExtent.Max[0] - this->GridExtent.Min[0],
                           this->GridExtent.Max[1] - this->GridExtent.Min[1]);
        // spacing of a grid bucket
        dax::Vector2 spacing(range[0] / this->GridDivisions[0],
                             range[1] / this->GridDivisions[1]);
        // bin the point
        dax::Id3 id;
        id[0] = fabs(spacing[0]) < 0.0001 ?
                0 : dax::math::Floor(coord[0] / spacing[0]);
        id[1] = fabs(spacing[1]) < 0.0001 ?
                0 : dax::math::Floor(coord[1] / spacing[1]);
        return id;
    }
};

// main
int main(void)
{
    // first generate the triangle mesh
    TriangleMesh2DGenerator generator;
    generator.setExtent(0, 5.99, 0, 5.99);
    generator.setDivisions(3, 3);
    generator.generate();

    // points and connections
    std::vector<dax::Vector2> points = generator.getDaxPoints();
    std::vector<dax::Id> connections = generator.getDaxConnections();
    ArrayHandle<dax::Vector2> hPoints = make_ArrayHandle(points);
    ArrayHandle<dax::Id> hConnections = make_ArrayHandle(connections);

    // construct uniform grid and find out the bucket boundarys
    dax::Extent3 gridExtent(dax::make_Id3(0, 0, 0), dax::make_Id3(6, 6, 0));
    dax::Id3 gridDivisions(2, 2, 1);

    // compute bounding box for each triangle,
    // find out buckets that each triangle overlaps
    int triangleCount = hConnections.GetNumberOfValues() / 3;
    ArrayHandle<int> hOverlappingBucketCounts;
    ArrayHandle<dax::Vector2> hLowerLefts;
    ArrayHandle<dax::Vector2> hUpperRights;
    CountOverlappingBuckets countOverlappingBuckets(
            hPoints.PrepareForInput(),
            hConnections.PrepareForInput(),
            gridExtent,
            gridDivisions,
            hOverlappingBucketCounts.PrepareForOutput(triangleCount),
            hLowerLefts.PrepareForOutput(triangleCount),
            hUpperRights.PrepareForOutput(triangleCount));
    Algorithm::Schedule(countOverlappingBuckets, triangleCount);

    // output from device
    std::vector<int> overlappingBucketCounts
        (hOverlappingBucketCounts.GetNumberOfValues());
    hOverlappingBucketCounts.CopyInto(overlappingBucketCounts.begin());
    std::vector<dax::Vector2> lowerLefts(hLowerLefts.GetNumberOfValues());
    hLowerLefts.CopyInto(lowerLefts.begin());
    std::vector<dax::Vector2> upperRights(hUpperRights.GetNumberOfValues());
    hUpperRights.CopyInto(upperRights.begin());

    // total overlapping bucket count.
    ArrayHandle<int> hScanBucketCounts;
    int totalBucketCount = Algorithm::ScanExclusive(hOverlappingBucketCounts,
                                                    hScanBucketCounts);

    // print
    std::cout.precision(2);
    std::cout << std::fixed;
    std::cout << std::setw(10) << "No. Val: "
              << hOverlappingBucketCounts.GetNumberOfValues() << std::endl;
    std::cout << std::setw(10) << "Overlap: ";
    for (unsigned int i = 0; i < overlappingBucketCounts.size(); ++i)
        std::cout << std::setw(3) << overlappingBucketCounts[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "LL X: ";
    for (unsigned int i = 0; i < lowerLefts.size(); ++i)
        std::cout << std::setw(6) << lowerLefts[i][0] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "LL Y: ";
    for (unsigned int i = 0; i < lowerLefts.size(); ++i)
        std::cout << std::setw(6) << lowerLefts[i][1] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "UR X: ";
    for (unsigned int i = 0; i < upperRights.size(); ++i)
        std::cout << std::setw(6) << upperRights[i][0] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "UR Y: ";
    for (unsigned int i = 0; i < upperRights.size(); ++i)
        std::cout << std::setw(6) << upperRights[i][1] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Ttl Cnt: " << totalBucketCount << std::endl;
    
    // construct cell id to bucket id map
    ArrayHandle<dax::Id> hTriangleIds;
    ArrayHandle<dax::Id> hOverlappingBucketIds;
    BinTriangles binTriangles(
            hPoints.PrepareForInput(),
            hConnections.PrepareForInput(),
            hScanBucketCounts.PrepareForInput(),
            gridExtent,
            gridDivisions,
            hTriangleIds.PrepareForOutput(totalBucketCount),
            hOverlappingBucketIds.PrepareForOutput(totalBucketCount));
    Algorithm::Schedule(binTriangles, triangleCount);

    // output from device
    std::vector<dax::Id> triangleIds(hTriangleIds.GetNumberOfValues());
    std::vector<dax::Id> overlappingBucketIds
        (hOverlappingBucketIds.GetNumberOfValues());
    hTriangleIds.CopyInto(triangleIds.begin());
    hOverlappingBucketIds.CopyInto(overlappingBucketIds.begin());

    // print
    std::cout << std::setw(10) << "Triangle: ";
    for (int i = 0; i < triangleIds.size(); ++i)
        std::cout << std::setw(3) << triangleIds[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Buckets: ";
    for (int i = 0; i < overlappingBucketIds.size(); ++i)
        std::cout << std::setw(3) << overlappingBucketIds[i] << ",";
    std::cout << std::endl;

    // sort the map according the bucket ids
    ArrayHandle<dax::Id> hSortTriangleIds;
    ArrayHandle<dax::Id> hSortBucketIds;
    Algorithm::Copy(hTriangleIds, hSortTriangleIds);
    Algorithm::Copy(hOverlappingBucketIds, hSortBucketIds);
    Algorithm::SortByKey(hSortBucketIds, hSortTriangleIds);

    // output after sorting
    std::vector<dax::Id> sortTriangles(hSortTriangleIds.GetNumberOfValues());
    std::vector<dax::Id> sortBuckets(hSortBucketIds.GetNumberOfValues());
    hSortTriangleIds.CopyInto(sortTriangles.begin());
    hSortBucketIds.CopyInto(sortBuckets.begin());

    // print
    std::cout << std::setw(10) << "Sort Bkt: ";
    for (int i = 0; i < sortBuckets.size(); ++i)
        std::cout << std::setw(3) << sortBuckets[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Sort Tri: ";
    for (int i = 0; i < sortTriangles.size(); ++i)
        std::cout << std::setw(3) << sortTriangles[i] << ",";
    std::cout << std::endl;

    // reduce bucket ids
    ArrayHandle<dax::Id> hUniqueBucketIds;
    Algorithm::Copy(hSortBucketIds, hUniqueBucketIds);
    Algorithm::Unique(hUniqueBucketIds);
    ArrayHandle<dax::Id> hTriangleStartIds;
    Algorithm::LowerBounds(hSortBucketIds, hUniqueBucketIds, hTriangleStartIds);
    ArrayHandle<int> hBucketTriangleCounts;
    dax::Id numUniqueKeys = hUniqueBucketIds.GetNumberOfValues();
    Offset2CountFunctor offset2Count(
            hTriangleStartIds.PrepareForInput(),
            hBucketTriangleCounts.PrepareForOutput(numUniqueKeys),
            numUniqueKeys - 1,
            hSortBucketIds.GetNumberOfValues());
    Algorithm::Schedule(offset2Count, numUniqueKeys);

    ArrayHandle<dax::Id> hTriangleStarts;
    ArrayHandle<int> hTriangleCounts;
    int cellCount = gridDivisions[0] * gridDivisions[1];
    ArrayHandleConstant<dax::Id> hTriangleStartInit(-1, cellCount);
    Algorithm::Copy(hTriangleStartInit, hTriangleStarts);
    ArrayHandleConstant<int> hTriangleCountInit(0, cellCount);
    Algorithm::Copy(hTriangleCountInit, hTriangleCounts);
    Coarse2ImplicitFunctor coarse2Implicit(
            hUniqueBucketIds.PrepareForInput(),
            hTriangleStartIds.PrepareForInput(),
            hBucketTriangleCounts.PrepareForInput(),
            hTriangleStarts.PrepareForOutput(cellCount),
            hTriangleCounts.PrepareForOutput(cellCount));
    Algorithm::Schedule(coarse2Implicit, numUniqueKeys);

    // output from device
    std::vector<int> uniqueBucketIds(hUniqueBucketIds.GetNumberOfValues());
    std::vector<int> bucketTriangleCounts(hBucketTriangleCounts.GetNumberOfValues());
    std::vector<dax::Id> triangleStarts(hTriangleStarts.GetNumberOfValues());
    std::vector<int> triangleCounts(hTriangleCounts.GetNumberOfValues());
    hUniqueBucketIds.CopyInto(uniqueBucketIds.begin());
    hBucketTriangleCounts.CopyInto(bucketTriangleCounts.begin());
    hTriangleStarts.CopyInto(triangleStarts.begin());
    hTriangleCounts.CopyInto(triangleCounts.begin());

    // print
    std::cout << std::setw(10) << "Unique: ";
    for (int i = 0; i < uniqueBucketIds.size(); ++i)
        std::cout << std::setw(3) << uniqueBucketIds[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Counts: ";
    for (int i = 0; i < bucketTriangleCounts.size(); ++i)
        std::cout << std::setw(3) << bucketTriangleCounts[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Starts: ";
    for (int i = 0; i < triangleStarts.size(); ++i)
        std::cout << std::setw(3) << triangleStarts[i] << ",";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Counts: ";
    for (int i = 0; i < triangleCounts.size(); ++i)
        std::cout << std::setw(3) << triangleCounts[i] << ",";
    std::cout << std::endl;

    return 0;
}
