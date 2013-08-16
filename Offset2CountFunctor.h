#ifndef __OFFSET2COUNTFUNCTOR_H__
#define __OFFSET2COUNTFUNCTOR_H__

#include <dax/exec/internal/WorkletBase.h>

// This functor subtracts the current value to the previous value for each value
struct Offset2CountFunctor : dax::exec::internal::WorkletBase
{
    ArrayHandle<dax::Id>::PortalConstExecution OffsetsPortal;
    ArrayHandle<int>::PortalExecution CountsPortal;
    dax::Id MaxId;
    dax::Id OffsetEnd;

    Offset2CountFunctor(
        ArrayHandle<dax::Id>::PortalConstExecution offsetsPortal,
        ArrayHandle<int>::PortalExecution countsPortal,
        dax::Id maxId,
        dax::Id offsetEnd)
      : OffsetsPortal(offsetsPortal),
        CountsPortal(countsPortal),
        MaxId(maxId),
        OffsetEnd(offsetEnd)
    {}

    DAX_EXEC_EXPORT
    void operator()(dax::Id index) const
    {
      dax::Id thisOffset = this->OffsetsPortal.Get(index);
      dax::Id nextOffset;
      if (index == this->MaxId)
        {
        nextOffset = this->OffsetEnd;
        }
      else
        {
        nextOffset = this->OffsetsPortal.Get(index+1);
        }
      this->CountsPortal.Set(index, nextOffset - thisOffset);
    }
};

#endif //__OFFSET2COUNTFUNCTOR_H__
