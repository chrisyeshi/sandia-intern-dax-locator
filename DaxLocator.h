#ifndef __DAXLOCATOR_H__
#define __DAXLOCATOR_H__

#include <dax/Types.h>
#include <dax/cont/UniformGrid.h>
#include <dax/math/Precision.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/DeviceAdapter.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>

#include "ExecLocator.h"

using namespace dax::cont;

class DaxLocator
{
public:
    DaxLocator();
    virtual ~DaxLocator();

    void setSpacing(float x, float y);
    void setExtent(int xmin, int xmax, int ymin, int ymax);
    void setPoints(const std::vector<dax::Vector2>& points);
    void build();

    std::vector<dax::Vector2> getSortPoints() const;
    std::vector<dax::Id> getPointStarts() const;
    std::vector<int> getPointCounts() const;
    dax::Id locatePoint(const dax::Vector2& point) const;
    std::vector<dax::Vector2> getBucketPoints(const dax::Id& bucketId) const;
    ExecLocator prepareExecutionObject() const;

protected:
    UniformGrid<> grid;
    ArrayHandle<dax::Id> hOriBucketIds;
    ArrayHandle<dax::Id> hBucketIds;
    ArrayHandle<dax::Vector2> hSortPoints;
    ArrayHandle<dax::Id> hUniqueBucketIds;
    ArrayHandle<dax::Id> hPointStartIds;
    ArrayHandle<int> hBucketPointCounts;
    ArrayHandle<dax::Id> hPointStarts;
    ArrayHandle<dax::Id> hPointCounts;

    ArrayHandle<dax::Id> mapPoints2Bin();
    ArrayHandle<dax::Id> sortPoints(ArrayHandle<dax::Id> hOriBucketIds);
    void formatBucketIds(ArrayHandle<dax::Id> hBucketIds);

    dax::Vector2 origin() const;
    dax::Vector2 spacing() const;
    dax::Extent3 extent() const;

    // temporary using a local struct to calculate the point count in each bucket
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

        void operator()(dax::Id index) const
        {
            // get the bucket id from uniqueBucketIds
            dax::Id bucketId = this->hUniqueBucketIds.Get(index);
            // then use bucketId to index the output arrays 
            this->hPointStarts.Set(bucketId, this->hPointStartIds.Get(index));
            this->hPointCounts.Set(bucketId, this->hBucketPointCounts.Get(index));
        }
    };

private:
    // CPU version of mapping a single point into a bin
    dax::Id binPoint(const dax::Vector2& point) const;
};

#endif //__DAXLOCATOR_H__
