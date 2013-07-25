#ifndef __BINPOINTS_H__
#define __BINPOINTS_H__

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/math/Precision.h>
#include <dax/exec/WorkletMapField.h>

namespace dax {
namespace worklet {

class BinPoints : public dax::exec::WorkletMapField
{
public:
    // control signature and execution signature
    typedef void ControlSignature(Field(In), Field(In), Field(In), Field(In),
                                  Field(Out));
    typedef _5 ExecutionSignature(_1, _2, _3, _4);

    DAX_EXEC_EXPORT
    dax::Id operator()(const dax::Vector3& point,
                       const dax::Vector3& origin,
                       const dax::Vector3& spacing,
                       const dax::Extent3& extent) const
    {
        return bin(point, origin, spacing, extent);
    }

    DAX_EXEC_CONT_EXPORT
    dax::Id bin(const dax::Vector3& point,
                const dax::Vector3& origin,
                const dax::Vector3& spacing,
                const dax::Extent3& extent) const
    {
        dax::Id3 dimensions = extentCellDimensions(extent);
        dax::Id3 divs(dax::Scalar(dimensions[0]) / spacing[0] + 0.5,
                      dax::Scalar(dimensions[1]) / spacing[1] + 0.5,
                      dax::Scalar(dimensions[2]) / spacing[2] + 0.5);
        // compute the point coordinate within the grid
        dax::Vector3 coord(point[0] - origin[0],
                           point[1] - origin[1],
                           point[2] - origin[2]);
        // which bucket the point belongs
        dax::Id id[3];
        for (int i = 0; i < 3; ++i)
            id[i] = fabs(spacing[i]) < 0.0001 ?
                0 : dax::math::Floor(coord[i] / spacing[i]);
        return id[0] + id[1] * divs[0] + id[2] * divs[0] * divs[2];
    }
};

}
}

#endif //__BINPOINTS_H__
