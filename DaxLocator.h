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

    void setSpacing(float x, float y, float z);
    void setExtent(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax);
    void setPoints(const std::vector<dax::Vector3>& points);
    void build();

    std::vector<dax::Vector3> getSortPoints() const;
    std::vector<dax::Id> getPointStarts() const;
    std::vector<int> getPointCounts() const;
    dax::Id locatePoint(const dax::Vector3& point) const;
    std::vector<dax::Vector3> getBucketPoints(const dax::Id& bucketId) const;
    ExecLocator prepareExecutionObject() const;

protected:
    UniformGrid<> grid;
    ArrayHandle<dax::Id> hOriBucketIds;
    ArrayHandle<dax::Id> hBucketIds;
    ArrayHandle<dax::Vector3> hSortPoints;
    ArrayHandle<dax::Id> hUniqueBucketIds;
    ArrayHandle<dax::Id> hPointStartIds;
    ArrayHandle<int> hBucketPointCounts;
    ArrayHandle<dax::Id> hPointStarts;
    ArrayHandle<dax::Id> hPointCounts;

    ArrayHandle<dax::Id> mapPoints2Bin();
    ArrayHandle<dax::Id> sortPoints(ArrayHandle<dax::Id> hOriBucketIds);
    void formatBucketIds(ArrayHandle<dax::Id> hBucketIds);
    dax::Id binPoint(const dax::Vector3& point) const;

    dax::Vector3 origin() const;
    dax::Vector3 spacing() const;
    dax::Extent3 extent() const;

private:
};

#endif //__DAXLOCATOR_H__
