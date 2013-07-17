#include <iostream>

#include <dax/Types.h>
#include <dax/exec/WorkletMapField.h>

using namespace dax::cont;

class MapPointsToGrid : public dax::exec::WorkletMapField
{
public:
    typedef void ControlSignature(Field(In), Field(Out));
    typedef void ExecutionSignature(_1, _2);

    DAX_EXEC_EXPORT
    void operator()(const dax::Vector2& coord,
                    const dax::Vector2& origin,
                    const dax::Vector2& spacing,
                    const dax::Extent2& extent,
                    dax::Scalar& cellId) const
    {
        
    }
}
