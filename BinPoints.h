#ifndef __BINPOINTS_H__
#define __BINPOINTS_H__

#include <iostream>

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/math/Precision.h>
#include <dax/exec/WorkletMapField.h>

#include "MapPointToBucket.h"

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
        return MapPointToBucket(origin, spacing, extent).MapToFlatIndex(point);
    }
};

}
}

#endif //__BINPOINTS_H__
