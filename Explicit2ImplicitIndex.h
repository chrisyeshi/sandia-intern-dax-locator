#include <dax/exec/internal/WorkletBase.h>
#include <dax/cont/ArrayHandle.h>

#include <iostream>

using namespace dax::cont;

// functor to translate from coarse representation to implicit representation
// of the grid ids
template<typename T>
struct Explicit2ImplicitIndex : dax::exec::internal::WorkletBase
{
    ArrayHandle<dax::Id>::PortalConstExecution hExplicitIndices;
    typename ArrayHandle<T>::PortalConstExecution hExplicitValues;
    typename ArrayHandle<T>::PortalExecution hImplicitValues;

    Explicit2ImplicitIndex(
        ArrayHandle<dax::Id>::PortalConstExecution hExplicitIndices,
        typename ArrayHandle<T>::PortalConstExecution hExplicitValues,
        typename ArrayHandle<T>::PortalExecution hImplicitValues)
      : hExplicitIndices(hExplicitIndices),
        hExplicitValues(hExplicitValues),
        hImplicitValues(hImplicitValues)
    {}

    DAX_EXEC_EXPORT
    void operator()(dax::Id index) const
    {
        // get the index from explicit index array
        dax::Id explicitIndex = this->hExplicitIndices.Get(index);
        // then use explicitIndex to index the output implicit values array
        this->hImplicitValues.Set(explicitIndex, this->hExplicitValues.Get(index));
    }
};
