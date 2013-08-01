#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <vector>
#include <cassert>

#include <dax/Types.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>
#include <dax/cont/ArrayHandleConstant.h>

#include "Explicit2ImplicitIndex.h"

using namespace dax::cont;

typedef dax::cont::internal::DeviceAdapterAlgorithm<DAX_DEFAULT_DEVICE_ADAPTER_TAG> Algorithm;

// main
int main(void)
{
    dax::Id indicesInit[] = {3, 8, 9};
    std::vector<dax::Id> explicitIndices(indicesInit,
            indicesInit + sizeof(indicesInit) / sizeof(indicesInit[0]));
    ArrayHandle<dax::Id> hExplicitIndices = make_ArrayHandle(explicitIndices);

    dax::Id valuesInit[] = {123, 321, 456};
    std::vector<dax::Id> explicitValues(valuesInit,
            valuesInit + sizeof(valuesInit) / sizeof(valuesInit[0]));
    ArrayHandle<dax::Id> hExplicitValues = make_ArrayHandle(explicitValues);

    // results ArrayHandle
    ArrayHandleConstant<dax::Id> hImplicitValuesInit(-1, 10);
    ArrayHandle<dax::Id> hImplicitValues;
    Algorithm::Copy(hImplicitValuesInit, hImplicitValues);

    std::cout << hExplicitIndices.GetNumberOfValues() << std::endl;
    // use Explicit2ImplicitIndex functor
    Explicit2ImplicitIndex<dax::Id> convert(hExplicitIndices.PrepareForInput(),
                                            hExplicitValues.PrepareForInput(),
                                            hImplicitValues.PrepareForOutput(10));
    Algorithm::Schedule(convert, hExplicitIndices.GetNumberOfValues());

    std::cout << 2 << std::endl;
    // verify output
    std::vector<dax::Id> implicitValues(hImplicitValues.GetNumberOfValues());
    hImplicitValues.CopyInto(implicitValues.begin());

    // expected implict values
    dax::Id correctImplictValuesInit[] = {-1, -1, -1, 123, -1, -1, -1, -1, 321, 456};
    std::vector<dax::Id> correctImplicitValues(correctImplictValuesInit,
            correctImplictValuesInit + sizeof(correctImplictValuesInit) / sizeof(correctImplictValuesInit[0]));

    assert(implicitValues.size() == correctImplicitValues.size());
    for (unsigned int i = 0; i < implicitValues.size(); ++i)
        assert(implicitValues[i] == correctImplicitValues[i]);
    
    return 0;
}
