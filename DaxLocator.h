#ifndef __DAXLOCATOR_H__
#define __DAXLOCATOR_H__

#include <dax/Types.h>
#include <dax/cont/UniformGrid.h>
#include <dax/math/Precision.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/DeviceAdapter.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>
#include <dax/exec/WorkletMapField.h>

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

private:
    // CPU version of mapping a single point into a bin
    dax::Id binPoint(const dax::Vector2& point) const;
};

#endif //__DAXLOCATOR_H__
