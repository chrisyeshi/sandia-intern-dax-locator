#ifndef __DAXLOCATOR_H__
#define __DAXLOCATOR_H__

#include <dax/Types.h>
#include <dax/cont/UniformGrid.h>
#include <dax/math/Precision.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/DeviceAdapter.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>

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

    std::vector<dax::Id> getOriBucketIds() const;
    std::vector<dax::Id> getBucketIds() const;
    std::vector<dax::Vector2> getPoints() const;
    std::vector<dax::Vector2> getSortPoints() const;
    std::vector<dax::Id> getUniqueBucketIds() const;
    std::vector<dax::Id> getPointStartIds() const;
    std::vector<int> getBucketPointCounts() const;
    dax::Id locatePoint(const dax::Vector2& point) const;
    std::vector<dax::Vector2> getBucketPoints(const dax::Id& bucketId) const;

protected:
    ArrayHandle<dax::Vector2> hPoints;
    UniformGrid<> grid;
    ArrayHandle<dax::Id> hOriBucketIds;
    ArrayHandle<dax::Id> hBucketIds;
    ArrayHandle<dax::Vector2> hSortPoints;
    ArrayHandle<dax::Id> hUniqueBucketIds;
    ArrayHandle<dax::Id> hPointStartIds;
    ArrayHandle<int> hBucketPointCounts;

    void mapPoints2Bin();
    void sortPoints();
    void formatBucketIds();

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
            OffsetEnd(offsetEnd) {  }

        void operator()(dax::Id index) const {
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

private:
    // CPU version of mapping a single point into a bin
    dax::Id binPoint(const dax::Vector2& point) const;
};

#endif //__DAXLOCATOR_H__
