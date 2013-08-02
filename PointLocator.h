#ifndef __POINTLOCATOR_H__
#define __POINTLOCATOR_H__

#include <dax/Types.h>
#include <dax/cont/UniformGrid.h>
#include <dax/math/Precision.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/DeviceAdapter.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>
#include <dax/exec/WorkletMapField.h>

#include "PointLocatorExec.h"

using namespace dax::cont;

class PointLocator
{
public:
    PointLocator();
    virtual ~PointLocator();

    void setAutomatic(bool automatic) { this->automatic = automatic; }
    void setPointsPerBucket(int ppb) { pointsPerBucket = ppb; }
    void setDimensions(int x, int y, int z);
    void setBounds(float x, float y, float z);
    void setPoints(const std::vector<dax::Vector3>& points);
    void build();

    // smart getter function that only becomes valid
    // after you setPoints,
    // it returns the number of cells per dimension depending on
    // whether it's automatic mode or manual mode
    dax::Id3 getDimensions() const { return dimensions(); }
    std::vector<dax::Vector3> getSortPoints() const;
    std::vector<dax::Id> getPointStarts() const;
    std::vector<int> getPointCounts() const;
    dax::Id locatePoint(const dax::Vector3& point) const;
    std::vector<dax::Vector3> getBucketPoints(const dax::Id& bucketId) const;
    PointLocatorExec prepareExecutionObject() const;

protected:
    // when automatic is true,
    // grid resolution is calculated automatically,
    // when false, user needs to specify the grid spacing
    // default to be true
    bool automatic;

    // average points per bucket,
    // used to compute the grid resolution in automatic mode.
    // defaut to be 3
    int pointsPerBucket;

    dax::Vector3 Bounds;
    dax::Id3 Dimensions;
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

    dax::Vector3 bounds() const;
    dax::Id3 dimensions() const;
    int bucketCount() const;
    UniformGrid<> grid() const;

private:
};

#endif //__POINTLOCATOR_H__
