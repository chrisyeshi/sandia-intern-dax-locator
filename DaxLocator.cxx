#include "DaxLocator.h"

#include <iostream>
#include <iomanip>

#include <dax/cont/ArrayHandleConstant.h>

#include "BinPoints.h"

typedef dax::cont::internal::DeviceAdapterAlgorithm<DAX_DEFAULT_DEVICE_ADAPTER_TAG> Algorithm;

//////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
// Public Methods
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

DaxLocator::DaxLocator()
{
}

DaxLocator::~DaxLocator()
{
}

void DaxLocator::setSpacing(float x, float y)
{
    this->grid.SetSpacing(dax::make_Vector3(x, y, 0.0));
}

void DaxLocator::setExtent(int xmin, int xmax, int ymin, int ymax)
{
    this->grid.SetOrigin(dax::make_Vector3(float(xmin), float(ymin), 0.0));
    this->grid.SetExtent(dax::make_Id3(xmin, ymin, 0), dax::make_Id3(xmax, ymax, 1));
}

void DaxLocator::setPoints(const std::vector<dax::Vector2>& points)
{
    this->hPoints = make_ArrayHandle(points);
}

void DaxLocator::build()
{
    // Step 1: map points to bin
    mapPoints2Bin();
    // Step 2: sort points according to bucket ids
    sortPoints();
    // Step 3: turn the bucketIds array into offset and count format
    // bucketIds: [1, 1, 2, 2, 2, 3]
    // ==>
    // offset: [0, 2, 5]
    // count: [2, 3, 1]
    formatBucketIds();
}

std::vector<dax::Id> DaxLocator::getOriBucketIds() const
{
    std::vector<dax::Id> oriBucketIds(hOriBucketIds.GetNumberOfValues());
    hOriBucketIds.CopyInto(oriBucketIds.begin());
    return oriBucketIds;
}

std::vector<dax::Id> DaxLocator::getBucketIds() const
{
    std::vector<dax::Id> bucketIds(hBucketIds.GetNumberOfValues());
    hBucketIds.CopyInto(bucketIds.begin());
    return bucketIds;
}

std::vector<dax::Vector2> DaxLocator::getPoints() const
{
    std::vector<dax::Vector2> points(hPoints.GetNumberOfValues());
    hPoints.CopyInto(points.begin());
    return points;
}

std::vector<dax::Vector2> DaxLocator::getSortPoints() const
{
    std::vector<dax::Vector2> sortPoints(hSortPoints.GetNumberOfValues());
    hSortPoints.CopyInto(sortPoints.begin());
    return sortPoints;
}

std::vector<dax::Id> DaxLocator::getUniqueBucketIds() const
{
    std::vector<dax::Id> uniqueBucketIds(hUniqueBucketIds.GetNumberOfValues());
    hUniqueBucketIds.CopyInto(uniqueBucketIds.begin());
    return uniqueBucketIds;
}

std::vector<dax::Id> DaxLocator::getPointStartIds() const
{
    std::vector<dax::Id> pointStartIds(hPointStartIds.GetNumberOfValues());
    hPointStartIds.CopyInto(pointStartIds.begin());
    return pointStartIds;
}

std::vector<int> DaxLocator::getBucketPointCounts() const
{
    std::vector<int> bucketPointCounts(hBucketPointCounts.GetNumberOfValues());
    hBucketPointCounts.CopyInto(bucketPointCounts.begin());
    return bucketPointCounts;
}

dax::Id DaxLocator::locatePoint(const dax::Vector2& point) const
{
    // find the bucket id that the point belongs to
    dax::Id id = binPoint(point);
    return id;
}

std::vector<dax::Vector2> DaxLocator::getBucketPoints(const dax::Id& bucketId) const
{
    // variables
    std::vector<dax::Id> uniqueBucketIds = this->getUniqueBucketIds();
    std::vector<dax::Id> pointStartIds = this->getPointStartIds();
    std::vector<int> bucketPointCounts = this->getBucketPointCounts();
    // find the points in the same bucket
    std::vector<dax::Id>::iterator startItr
        = std::find(uniqueBucketIds.begin(), uniqueBucketIds.end(), bucketId);
    // make sure the bucket contains at least 1 point
    // if not, return an empty array
    if (startItr == uniqueBucketIds.end())
        return std::vector<dax::Vector2>();
    // if yes, get the index for the point arrays
    dax::Id uniqueIndex = std::distance(uniqueBucketIds.begin(), startItr);
    // staring point id and number of points in this bucket
    dax::Id start = pointStartIds[uniqueIndex];
    int count = bucketPointCounts[uniqueIndex];
    // construct the return points array
    std::vector<dax::Vector2> points(count);
    std::vector<dax::Vector2> sortPoints = this->getSortPoints();
    for (unsigned int i = 0; i < count; ++i)
        points[i] = sortPoints[i + start];
    return points;
}

//////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
// Pretected Methods
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

void DaxLocator::mapPoints2Bin()
{
    // use a worklet to find out which bin each point belongs to
    // results are stored in this->hOriBucketIds
    Scheduler<> scheduler;
    scheduler.Invoke(dax::worklet::BinPoints(),
                     hPoints,
                     origin(),
                     spacing(),
                     extent(),
                     this->hOriBucketIds);
}

void DaxLocator::sortPoints()
{
    // sort the point array according to the bucketIds array
    // use the sorting functions provided by dax
    // copy into the new variables
    Algorithm::Copy(this->hOriBucketIds, this->hBucketIds);
    Algorithm::Copy(this->hPoints, this->hSortPoints);
    // sort by key
    Algorithm::SortByKey(this->hBucketIds, this->hSortPoints);
}

void DaxLocator::formatBucketIds()
{
    // use the unique operator to find out the unique bucket ids that have points
    Algorithm::Copy(this->hBucketIds, this->hUniqueBucketIds);
    Algorithm::Unique(this->hUniqueBucketIds);
    // use lowerbound to find the starting positions of the ids
    Algorithm::LowerBounds(this->hBucketIds, this->hUniqueBucketIds, // inputs
                           this->hPointStartIds); // outputs

    // calculate the count of points in each bucket
    // apply the functor to find the counts
    dax::Id numUniqueKeys = hUniqueBucketIds.GetNumberOfValues();
    Offset2CountFunctor offset2Count(
            this->hPointStartIds.PrepareForInput(),
            this->hBucketPointCounts.PrepareForOutput(numUniqueKeys),
            numUniqueKeys - 1,
            this->hBucketIds.GetNumberOfValues());
    Algorithm::Schedule(offset2Count, numUniqueKeys);
}

dax::Vector2 DaxLocator::origin() const
{
    return dax::Vector2(grid.GetOrigin()[0], grid.GetOrigin()[1]);
}

dax::Vector2 DaxLocator::spacing() const
{
    return dax::Vector2(grid.GetSpacing()[0], grid.GetSpacing()[1]);
}

dax::Extent3 DaxLocator::extent() const
{
    return grid.GetExtent();
}

//////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
// Private Methods
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

dax::Id DaxLocator::binPoint(const dax::Vector2& point) const
{
    int resolution[2] = {extent().Max[0] - extent().Min[0],
                         extent().Max[1] - extent().Min[0]};
    // compute the point coordinate within the grid
    dax::Vector2 coord(point[0] - origin()[0], point[1] - origin()[1]);
    // which bucket the point belongs
    dax::Id xid, yid;
    xid = dax::math::Floor(coord[0] / spacing()[0]);
    yid = dax::math::Floor(coord[1] / spacing()[1]);
    return xid + yid * resolution[0];
}
