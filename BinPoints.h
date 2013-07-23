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
    dax::Id operator()(const dax::Vector2& point,
                       const dax::Vector2& origin,
                       const dax::Vector2& spacing,
                       const dax::Extent3& extent) const
    {
        int resolution[2] = {extent.Max[0] - extent.Min[0],
                             extent.Max[1] - extent.Min[0]};
        // compute the point coordinate within the grid
        dax::Vector2 coord(point[0] - origin[0], point[1] - origin[1]);
        // which cell the point belongs
        dax::Id xid, yid;
        xid = dax::math::Floor(coord[0] / spacing[0]);
        yid = dax::math::Floor(coord[1] / spacing[1]);
        return xid + yid * resolution[0];
    }
};

}
}

#endif //__BINPOINTS_H__
