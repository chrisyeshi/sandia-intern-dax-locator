#include <dax/exec/internal/WorkletBase.h>

// functor to translate from coarse representation to implicit representation
// of the grid ids
struct Coarse2ImplicitFunctor : dax::exec::internal::WorkletBase
{
    ArrayHandle<dax::Id>::PortalConstExecution hUniqueBucketIds;
    ArrayHandle<dax::Id>::PortalConstExecution hPointStartIds;
    ArrayHandle<dax::Id>::PortalConstExecution hBucketPointCounts;
    ArrayHandle<dax::Id>::PortalExecution hPointStarts;
    ArrayHandle<int>::PortalExecution hPointCounts;

    Coarse2ImplicitFunctor(
        ArrayHandle<dax::Id>::PortalConstExecution hUniqueBucketIds_in,
        ArrayHandle<dax::Id>::PortalConstExecution hPointStartIds_in,
        ArrayHandle<dax::Id>::PortalConstExecution hBucketPointCounts_in,
        ArrayHandle<dax::Id>::PortalExecution hPointStarts_in,
        ArrayHandle<int>::PortalExecution hPointCounts_in)
      : hUniqueBucketIds(hUniqueBucketIds_in),
        hPointStartIds(hPointStartIds_in),
        hBucketPointCounts(hBucketPointCounts_in),
        hPointStarts(hPointStarts_in),
        hPointCounts(hPointCounts_in)
    {}

    DAX_EXEC_EXPORT
    void operator()(dax::Id index) const
    {
        // get the bucket id from uniqueBucketIds
        dax::Id bucketId = this->hUniqueBucketIds.Get(index);
        // then use bucketId to index the output arrays 
        this->hPointStarts.Set(bucketId, this->hPointStartIds.Get(index));
        this->hPointCounts.Set(bucketId, this->hBucketPointCounts.Get(index));
    }
};
