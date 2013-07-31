#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/exec/internal/WorkletBase.h>

// This worklet parallelize on each cell,
// find what buckets each cell overlaps,
// then output cell ids to CellIds array,
// and bucket ids to BucketIds array,
// user needs to set CellIds and BucketIds prior to invoking the worklet.
class BinTriangles : public dax::exec::internal::WorkletBase
{
public:
    // inputs
    ArrayHandle<dax::Vector3>::PortalConstExecution PointsPortal;
    ArrayHandle<int>::PortalConstExecution ConnectionsPortal;
    ArrayHandle<int>::PortalConstExecution ScanBucketCounts;
    dax::Extent3 GridExtent;
    dax::Id3 GridDivisions;

    // the output arrays
    ArrayHandle<dax::Id>::PortalExecution CellIds;
    ArrayHandle<dax::Id>::PortalExecution BucketIds;

    // constructor
    BinTriangles(ArrayHandle<dax::Vector3>::PortalConstExecution pointsPortal,
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
        dax::Vector3 vertices[verticesPerTriangle];
        for (int i = 0; i < verticesPerTriangle; ++i)
        {
            vertexIds[i] = this->ConnectionsPortal
                .Get(index * verticesPerTriangle + i);
            vertices[i] = this->PointsPortal.Get(vertexIds[i]);
        }
        // bounding box of the triangle
        dax::Vector3 lowerLeft = vertices[0];
        dax::Vector3 upperRight = vertices[0];
        for (int i = 0; i < verticesPerTriangle; ++i)
        {
            lowerLeft = dax::math::Min<dax::Vector3>(lowerLeft, vertices[i]);
            upperRight = dax::math::Max<dax::Vector3>(upperRight, vertices[i]);
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
        int zRange = upperRightBucket[2] - lowerLeftBucket[2] + 1;
        for (int z = lowerLeftBucket[2]; z <= upperRightBucket[2]; ++z)
            for (int y = lowerLeftBucket[1]; y <= upperRightBucket[1]; ++y)
                for (int x = lowerLeftBucket[0]; x <=upperRightBucket[0]; ++x)
                {
                    // put the cell id and bucket id into their appropriate positions
                    dax::Id triangleId = index;
                    dax::Id bucketId = x + y * this->GridDivisions[0]
                        + z * this->GridDivisions[1] * this->GridDivisions[0];
                    dax::Id start = this->ScanBucketCounts.Get(triangleId);
                    dax::Id offset = (x - lowerLeftBucket[0])
                                   + (y - lowerLeftBucket[1]) * xRange
                                   + (z - lowerLeftBucket[2]) * xRange * yRange;
                    this->CellIds.Set(start + offset, triangleId);
                    this->BucketIds.Set(start + offset, bucketId);
                }
    }

    DAX_EXEC_EXPORT
    dax::Id3 binPoint(const dax::Vector3& point) const
    {
        // compute the point coordinate within the grid
        dax::Vector3 coord(point[0] - this->GridExtent.Min[0],
                           point[1] - this->GridExtent.Min[1],
                           point[2] - this->GridExtent.Min[2]);
        // ranges of the whole grid
        dax::Vector3 range(this->GridExtent.Max[0] - this->GridExtent.Min[0],
                           this->GridExtent.Max[1] - this->GridExtent.Min[1],
                           this->GridExtent.Max[2] - this->GridExtent.Min[2]);
        // spacing of a grid bucket
        dax::Vector3 spacing(range[0] / this->GridDivisions[0],
                             range[1] / this->GridDivisions[1],
                             range[2] / this->GridDivisions[2]);
        // bin the point
        dax::Id3 id;
        id[0] = fabs(spacing[0]) < 0.0001 ?
                0 : dax::math::Floor(coord[0] / spacing[0]);
        id[1] = fabs(spacing[1]) < 0.0001 ?
                0 : dax::math::Floor(coord[1] / spacing[1]);
        id[2] = fabs(spacing[2]) < 0.0001 ?
                0 : dax::math::Floor(coord[2] / spacing[2]);
        return id;
    }
};
