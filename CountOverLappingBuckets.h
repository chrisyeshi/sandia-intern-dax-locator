#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>
#include <dax/math/Precision.h>
#include <dax/exec/WorkletMapCell.h>

using namespace dax::cont;

struct CountOverlappingBuckets : dax::exec::WorkletMapCell
{
    ArrayHandle<dax::Vector2>::PortalConstExecution PointsPortal;
    ArrayHandle<int>::PortalConstExecution ConnectionsPortal;
    dax::Extent3 GridExtent;
    dax::Id3 GridDivisions;
    ArrayHandle<int>::PortalExecution BucketCountsPortal;
    ArrayHandle<dax::Vector2>::PortalExecution LowerLeftsPortal;
    ArrayHandle<dax::Vector2>::PortalExecution UpperRightsPortal;

    CountOverlappingBuckets(
            ArrayHandle<dax::Vector2>::PortalConstExecution pointsPortal,
            ArrayHandle<int>::PortalConstExecution connectionsPortal,
            dax::Extent3 gridExtent,
            dax::Id3 gridDivisions,
            ArrayHandle<int>::PortalExecution bucketCountsPortal,
            ArrayHandle<dax::Vector2>::PortalExecution lowerLeftsPortal,
            ArrayHandle<dax::Vector2>::PortalExecution upperRightsPortal)
          : PointsPortal(pointsPortal),
            ConnectionsPortal(connectionsPortal),
            GridExtent(gridExtent),
            GridDivisions(gridDivisions),
            BucketCountsPortal(bucketCountsPortal),
            LowerLeftsPortal(lowerLeftsPortal),
            UpperRightsPortal(upperRightsPortal)
    {}

    DAX_EXEC_EXPORT
    void operator()(dax::Id index) const
    {
        // ths index here is the index of the triangles
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
        // make bounding box of this triangle
        dax::Vector2 lowerLeft = vertices[0];
        dax::Vector2 upperRight = vertices[0];
        for (int i = 1; i < verticesPerTriangle; ++i)
        {
            lowerLeft = dax::math::Min<dax::Vector2>(lowerLeft, vertices[i]);
            upperRight = dax::math::Max<dax::Vector2>(upperRight, vertices[i]);
        }
        // output the bounding box for debugging
        this->LowerLeftsPortal.Set(index, lowerLeft);
        this->UpperRightsPortal.Set(index, upperRight);
        // using the bounding box to estimate the number of overlapping buckets
        // find the bucket that the lower left cornor of the bounding box is in
        // find the bucket that the upper right cornor of the bounding box is in
        // they both define the quad of buckets the triangle overlaps
        dax::Id3 lowerLeftBucket = binPoint(lowerLeft);
        dax::Id3 upperRightBucket = binPoint(upperRight);
        // draw a quad using these two bucket ids
        int xRange = upperRightBucket[0] - lowerLeftBucket[0] + 1;
        int yRange = upperRightBucket[1] - lowerLeftBucket[1] + 1;
        int count = xRange * yRange;

        this->BucketCountsPortal.Set(index, count);
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
