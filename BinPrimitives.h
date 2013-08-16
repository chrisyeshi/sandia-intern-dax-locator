#include <iostream>
#include <iomanip>

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/exec/internal/WorkletBase.h>
#include <dax/exec/internal/TopologyUniform.h>
#include <dax/CellTag.h>
#include <dax/CellTraits.h>

#include "MapPointToBucket.h"

// This worklet parallelize on each cell,
// find what buckets each cell overlaps,
// then output cell ids to CellIds array,
// and bucket ids to BucketIds array,
// user needs to set CellIds and BucketIds prior to invoking the worklet.
template <class CellTag>
class BinPrimitives : public dax::exec::internal::WorkletBase
{
public:
    typedef dax::exec::internal::TopologyUniform TopologyStructConstExecution;

    // inputs
    ArrayHandle<dax::Vector3>::PortalConstExecution PointsPortal;
    ArrayHandle<int>::PortalConstExecution ConnectionsPortal;
    ArrayHandle<int>::PortalConstExecution ScanBucketCounts;
    TopologyStructConstExecution Topology;

    // the output arrays
    ArrayHandle<dax::Id>::PortalExecution CellIds;
    ArrayHandle<dax::Id>::PortalExecution BucketIds;

    // constructor
    BinPrimitives(ArrayHandle<dax::Vector3>::PortalConstExecution pointsPortal,
                  ArrayHandle<int>::PortalConstExecution connectionsPortal,
                  ArrayHandle<int>::PortalConstExecution overlappingBucketCounts,
                  TopologyStructConstExecution topology,
                  ArrayHandle<dax::Id>::PortalExecution cellIds,
                  ArrayHandle<dax::Id>::PortalExecution bucketIds)
                : PointsPortal(pointsPortal),
                  ConnectionsPortal(connectionsPortal),
                  ScanBucketCounts(overlappingBucketCounts),
                  Topology(topology),
                  CellIds(cellIds),
                  BucketIds(bucketIds)
    {}

    DAX_EXEC_EXPORT
    void operator()(dax::Id index) const
    {
        // the index here is the index of the primitives
        // vertices of this primitive
        const int verticesPerPrimitive = dax::CellTraits<CellTag>::NUM_VERTICES;
        dax::Id vertexIds[verticesPerPrimitive];
        dax::Vector3 vertices[verticesPerPrimitive];
        for (int i = 0; i < verticesPerPrimitive; ++i)
        {
            vertexIds[i] = this->ConnectionsPortal
                .Get(index * verticesPerPrimitive + i);
            vertices[i] = this->PointsPortal.Get(vertexIds[i]);
        }
        // bounding box of the primitive
        dax::Vector3 lowerLeft = vertices[0];
        dax::Vector3 upperRight = vertices[0];
        for (int i = 1; i < verticesPerPrimitive; ++i)
        {
            lowerLeft = dax::math::Min<dax::Vector3>(lowerLeft, vertices[i]);
            upperRight = dax::math::Max<dax::Vector3>(upperRight, vertices[i]);
        }
        // using the bounding box to estimate the number of overlapping buckets
        // find the bucket that the lower left cornor of the bounding box is in
        // find the bucket that the upper right cornor of the bounding box is in
        // they both define the quad of buckets the primitive overlaps
        dax::Id3 lowerLeftBucket
            = MapPointToBucket(this->Topology).MapToIndex3(lowerLeft);
        dax::Id3 upperRightBucket
            = MapPointToBucket(this->Topology).MapToIndex3(upperRight);
        // draw a quad using these two bucket ids
        int xRange = upperRightBucket[0] - lowerLeftBucket[0] + 1;
        int yRange = upperRightBucket[1] - lowerLeftBucket[1] + 1;
        int zRange = upperRightBucket[2] - lowerLeftBucket[2] + 1;
        for (int z = lowerLeftBucket[2]; z <= upperRightBucket[2]; ++z)
            for (int y = lowerLeftBucket[1]; y <= upperRightBucket[1]; ++y)
                for (int x = lowerLeftBucket[0]; x <=upperRightBucket[0]; ++x)
                {
                    // put the cell id and bucket id into their appropriate positions
                    dax::Id primitiveId = index;
                    dax::Id bucketId = index3ToFlatIndexCell(dax::make_Id3(x, y, z),
                                                             this->Topology.Extent);
                    dax::Id start = this->ScanBucketCounts.Get(primitiveId);
                    dax::Id offset = (x - lowerLeftBucket[0])
                                   + (y - lowerLeftBucket[1]) * xRange
                                   + (z - lowerLeftBucket[2]) * xRange * yRange;
                    this->CellIds.Set(start + offset, primitiveId);
                    this->BucketIds.Set(start + offset, bucketId);
                }
    }
};
