#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>
#include <dax/math/Compare.h>
#include <dax/math/Precision.h>
#include <dax/exec/WorkletMapCell.h>
#include <dax/exec/internal/TopologyUniform.h>
#include <dax/CellTag.h>
#include <dax/CellTraits.h>

#include "MapPointToBucket.h"

using namespace dax::cont;

template <class CellTag>
struct CountOverlapBuckets : dax::exec::WorkletMapCell
{
    typedef dax::exec::internal::TopologyUniform TopologyStructConstExecution;

    ArrayHandle<dax::Vector3>::PortalConstExecution PointsPortal;
    ArrayHandle<int>::PortalConstExecution ConnectionsPortal;
    TopologyStructConstExecution Topology;
    ArrayHandle<int>::PortalExecution BucketCountsPortal;

    CountOverlapBuckets(
            ArrayHandle<dax::Vector3>::PortalConstExecution pointsPortal,
            ArrayHandle<int>::PortalConstExecution connectionsPortal,
            TopologyStructConstExecution topology,
            ArrayHandle<int>::PortalExecution bucketCountsPortal)
          : PointsPortal(pointsPortal),
            ConnectionsPortal(connectionsPortal),
            Topology(topology),
            BucketCountsPortal(bucketCountsPortal)
    {}

    DAX_EXEC_EXPORT
    void operator()(dax::Id index) const
    {
        // ths index here is the index of the triangles
        // vertices of this triangle
        const int numVertices = dax::CellTraits<CellTag>::NUM_VERTICES;
        dax::Id vertexIds[numVertices];
        dax::Vector3 vertices[numVertices];
        for (int i = 0; i < numVertices; ++i)
        {
            vertexIds[i] = this->ConnectionsPortal
                .Get(index * numVertices + i);
            vertices[i] = this->PointsPortal.Get(vertexIds[i]);
        }
        // make bounding box of this triangle
        dax::Vector3 lowerLeft = vertices[0];
        dax::Vector3 upperRight = vertices[0];
        for (int i = 1; i < numVertices; ++i)
        {
            lowerLeft = dax::math::Min<dax::Vector3>(lowerLeft, vertices[i]);
            upperRight = dax::math::Max<dax::Vector3>(upperRight, vertices[i]);
        }
        // using the bounding box to estimate the number of overlapping buckets
        // find the bucket that the lower left cornor of the bounding box is in
        // find the bucket that the upper right cornor of the bounding box is in
        // they both define the quad of buckets the triangle overlaps
        dax::Id3 lowerLeftBucket
            = MapPointToBucket(Topology).MapToIndex3(lowerLeft);
        dax::Id3 upperRightBucket
            = MapPointToBucket(Topology).MapToIndex3(upperRight);
        // draw a quad using these two bucket ids
        int xRange = upperRightBucket[0] - lowerLeftBucket[0] + 1;
        int yRange = upperRightBucket[1] - lowerLeftBucket[1] + 1;
        int zRange = upperRightBucket[2] - lowerLeftBucket[2] + 1;
        int count = xRange * yRange * zRange;

        this->BucketCountsPortal.Set(index, count);
    }
};
