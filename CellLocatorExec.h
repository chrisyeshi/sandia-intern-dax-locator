#ifndef __CELLLOCATOREXEC_H__
#define __CELLLOCATOREXEC_H__

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/cont/arg/ExecutionObject.h>
#include <dax/cont/UniformGrid.h>

#include "MapPointToBucket.h"
#include "PointInCell.h"

using namespace dax::cont;

template <class CellTag>
class CellLocatorExec : public dax::exec::ExecutionObjectBase
{
public:
    typedef dax::exec::internal::TopologyUniform TopologyStructConstExecution;
    typedef dax::exec::internal::TopologyUniform TopologyStructExecution;

    CellLocatorExec() {}
    CellLocatorExec(TopologyStructConstExecution topology,
                    ArrayHandle<dax::Vector3>::PortalConstExecution points,
                    ArrayHandle<dax::Id>::PortalConstExecution connections,
                    ArrayHandle<dax::Id>::PortalConstExecution sortCellIds,
                    ArrayHandle<dax::Id>::PortalConstExecution cellStarts,
                    ArrayHandle<int>::PortalConstExecution cellCounts)
                  : topology(topology),
                    points(points),
                    connections(connections),
                    sortCellIds(sortCellIds),
                    cellStarts(cellStarts),
                    cellCounts(cellCounts)
    {}

    void setTopology(TopologyStructConstExecution topology)
    { this->topology = topology; }
    void setPoints(ArrayHandle<dax::Vector3>::PortalConstExecution points)
    { this->points = points; }
    void setConnections(ArrayHandle<dax::Id>::PortalConstExecution connections)
    { this->connections = connections; }
    void setSortCellIds(ArrayHandle<dax::Id>::PortalConstExecution sortCellIds)
    { this->sortCellIds = sortCellIds; }
    void setCellStarts(ArrayHandle<dax::Id>::PortalConstExecution cellStarts)
    { this->cellStarts = cellStarts; }
    void setCellCounts(ArrayHandle<int>::PortalConstExecution cellCounts)
    { this->cellCounts = cellCounts; }

    // find bucket id that a point is in
    DAX_EXEC_EXPORT
    dax::Id getBucketId(const dax::Vector3& point) const
    {
        // make sure the point is within the bounds
        dax::Extent3 extent = this->topology.Extent;
        dax::Vector3 spacing = this->topology.Spacing;
        dax::Vector3 boundMin(extent.Min[0] * spacing[0],
                              extent.Min[1] * spacing[1],
                              extent.Min[2] * spacing[2]);
        dax::Vector3 boundMax(extent.Max[0] * spacing[0],
                              extent.Max[1] * spacing[1],
                              extent.Max[2] * spacing[2]);
        if (point[0] < boundMin[0] || point[0] > boundMax[0]
         || point[1] < boundMin[1] || point[1] > boundMax[1]
         || point[2] < boundMin[2] || point[2] > boundMax[2])
            return -1;
        return MapPointToBucket(topology).MapToFlatIndex(point);
    }

    // get bucket cell count by bucket id
    DAX_EXEC_EXPORT
    int getBucketCellCount(const dax::Id& bucketId) const
    {
        return cellCounts.Get(bucketId);
    }

    // find cell that contains the input point, returns the cell id
    DAX_EXEC_EXPORT
    dax::Id findCell(const dax::Vector3& point) const
    {
        // in which bucket
        dax::Id bucketId = this->getBucketId(point);
        dax::Id start = cellStarts.Get(bucketId);
        int count = cellCounts.Get(bucketId);
        // iterate to find the cell that contains the point
        for (int i = 0; i < count; ++i)
        {
            dax::Id sortCellId = start + i;
            dax::Id cellId = sortCellIds.Get(sortCellId);
            dax::exec::CellField<dax::Vector3, CellTag> cellVertices
                = this->getCell(cellId);
            if (PointInCell(point, cellVertices))
                return cellId;
        }
        // no cell is containing this point
        return -1;
    }

    // use cell id to get cell vertices
    DAX_EXEC_EXPORT
    dax::exec::CellField<dax::Vector3, CellTag> getCell(const dax::Id& cellId) const
    {
        dax::exec::CellField<dax::Vector3, CellTag> vertices;
        int numVertices = dax::CellTraits<CellTag>::NUM_VERTICES;
        for (int i = 0; i < numVertices; ++i)
        {
            dax::Id pointId = connections.Get(cellId * numVertices + i);
            vertices[i] = points.Get(pointId);
        }
        return vertices;
    }

protected:
    TopologyStructConstExecution topology;
    // points is referenced in connections, which follows the order of CellTag
    // a cell is represented in connections by (cell id * vertices per cell)
    // sortCellIds then use (cell id) to index hConnections
    // sortCellIds is the cell ids sorted by buckets,
    // there are replicates if a cell overlaps multiple buckets
    // cellStarts and cellCounts index the sortCellIds
    // so for each bucket, we have the start index in sortCellIds
    // and how many cells are in this bucket.
    ArrayHandle<dax::Vector3>::PortalConstExecution points;
    ArrayHandle<dax::Id>::PortalConstExecution connections;
    ArrayHandle<dax::Id>::PortalConstExecution sortCellIds;
    ArrayHandle<dax::Id>::PortalConstExecution cellStarts;
    ArrayHandle<int>::PortalConstExecution cellCounts;

private:
};

#endif //__CELLLOCATOREXEC_H__
