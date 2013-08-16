#ifndef __MAPPOINTTOBUCKET_H__
#define __MAPPOINTTOBUCKET_H__

#include <iostream>

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/CellTag.h>
#include <dax/cont/UniformGrid.h>
#include <dax/exec/internal/TopologyUniform.h>
#include <dax/math/Compare.h>
#include <dax/math/Precision.h>

using namespace dax::cont;

class MapPointToBucket
{
public:
    typedef dax::exec::internal::TopologyUniform TopologyStructConstExecution;
    typedef dax::exec::internal::TopologyUniform TopologyStructExecution;

    // the main parameters for the uniform grid
    dax::Vector3 Origin;
    dax::Vector3 Spacing;
    dax::Extent3 Extent;

    // some constructors to help
    DAX_EXEC_CONT_EXPORT
    MapPointToBucket() {}

    DAX_EXEC_CONT_EXPORT
    MapPointToBucket(const dax::Vector3& origin,
                         const dax::Vector3& spacing,
                         const dax::Extent3& extent)
                       : Origin(origin), Spacing(spacing), Extent(extent)
    {}

    DAX_CONT_EXPORT
    MapPointToBucket(const UniformGrid<>& grid)
      : Origin(grid.GetOrigin()),
        Spacing(grid.GetSpacing()),
        Extent(grid.GetExtent())
    {}

    DAX_EXEC_EXPORT
    MapPointToBucket(TopologyStructConstExecution topology)
      : Origin(topology.Origin),
        Spacing(topology.Spacing),
        Extent(topology.Extent)
    {}

    DAX_EXEC_CONT_EXPORT
    dax::Id3 MapToIndex3(const dax::Vector3& point) const
    {
        // compute the point coordinate within the grid
        dax::Vector3 coord(point[0] - this->Origin[0],
                           point[1] - this->Origin[1],
                           point[2] - this->Origin[2]);
        // which bucket the point belongs
        dax::Id3 id;
        for (int i = 0; i < 3; ++i)
            id[i] = fabs(this->Spacing[i]) < 0.0001 ?
                0 : dax::math::Floor(coord[i] / this->Spacing[i]);
        return id;
    }

    DAX_EXEC_CONT_EXPORT
    dax::Id MapToFlatIndex(const dax::Vector3& point) const
    {
        dax::Id3 id = MapToIndex3(point);
        dax::Id ret = index3ToFlatIndexCell(id, this->Extent);
        return ret;
    }

protected:

private:
};

#endif //__MAPPOINTTOBUCKET_H__
