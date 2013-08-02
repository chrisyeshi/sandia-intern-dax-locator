#define BOOST_SP_DISABLE_THREADS

#include "PointLocator.h"

#include <iostream>
#include <iomanip>

#include <dax/cont/ArrayHandleConstant.h>

#include "BinPoints.h"
#include "Offset2CountFunctor.h"
#include "Explicit2ImplicitIndex.h"
#include "MapPointToBucket.h"

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

PointLocator::PointLocator() : automatic(true), pointsPerBucket(3)
{
}

PointLocator::~PointLocator()
{
}

void PointLocator::setDimensions(int x, int y, int z)
{
    this->Dimensions[0] = x;
    this->Dimensions[1] = y;
    this->Dimensions[2] = z;
}

void PointLocator::setBounds(float x, float y, float z)
{
    this->Bounds[0] = x;
    this->Bounds[1] = y;
    this->Bounds[2] = z;
}

void PointLocator::setPoints(const std::vector<dax::Vector3>& points)
{
    this->hSortPoints = make_ArrayHandle(points);
}

void PointLocator::build()
{
    // Step 1: map points to bin
    ArrayHandle<dax::Id> hOriBucketIds = mapPoints2Bin();
    // Step 2: sort points according to bucket ids
    ArrayHandle<dax::Id> hBucketIds = sortPoints(hOriBucketIds);
    // Step 3: turn the bucketIds array into offset and count format
    formatBucketIds(hBucketIds);
}

std::vector<dax::Vector3> PointLocator::getSortPoints() const
{
    std::vector<dax::Vector3> sortPoints(hSortPoints.GetNumberOfValues());
    hSortPoints.CopyInto(sortPoints.begin());
    return sortPoints;
}

std::vector<dax::Id> PointLocator::getPointStarts() const
{
    std::vector<dax::Id> pointStarts(hPointStarts.GetNumberOfValues());
    hPointStarts.CopyInto(pointStarts.begin());
    return pointStarts;
}

std::vector<int> PointLocator::getPointCounts() const
{
    std::vector<int> pointCounts(hPointCounts.GetNumberOfValues());
    hPointCounts.CopyInto(pointCounts.begin());
    return pointCounts;
}

dax::Id PointLocator::locatePoint(const dax::Vector3& point) const
{
    // find the bucket id that the point belongs to
    dax::Id id = binPoint(point);
    return id;
}

std::vector<dax::Vector3> PointLocator::getBucketPoints(const dax::Id& bucketId) const
{
    // variables
    std::vector<dax::Id> pointStarts = this->getPointStarts();
    std::vector<int> pointCounts = this->getPointCounts();
    // get point start location and count
    dax::Id start = pointStarts[bucketId];
    int count = pointCounts[bucketId];
    // construct the return points array
    std::vector<dax::Vector3> points(count);
    std::vector<dax::Vector3> sortPoints = this->getSortPoints();
    for (unsigned int i = 0; i < count; ++i)
        points[i] = sortPoints[i + start];
    return points;
}

PointLocatorExec PointLocator::prepareExecutionObject() const
{
    PointLocatorExec ret;
    ret.setTopology(this->grid().PrepareForInput());
    ret.setSortPoints(this->hSortPoints.PrepareForInput());
    ret.setPointStarts(this->hPointStarts.PrepareForInput());
    ret.setPointCounts(this->hPointCounts.PrepareForInput());
    return ret;
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

ArrayHandle<dax::Id> PointLocator::mapPoints2Bin()
{
    // use a worklet to find out which bin each point belongs to
    // results are stored in this->hOriBucketIds
    ArrayHandle<dax::Id> hOriBucketIds;
    Scheduler<> scheduler;
    scheduler.Invoke(dax::worklet::BinPoints(),
                     this->hSortPoints,
                     this->grid().GetOrigin(),
                     this->grid().GetSpacing(),
                     this->grid().GetExtent(),
                     hOriBucketIds);

    return hOriBucketIds;
}

ArrayHandle<dax::Id> PointLocator::sortPoints(ArrayHandle<dax::Id> hOriBucketIds)
{
    // sort the point array according to the bucketIds array
    // use the sorting functions provided by dax
    // copy into the new variables
    ArrayHandle<dax::Id> hBucketIds;
    Algorithm::Copy(hOriBucketIds, hBucketIds);
    Algorithm::Copy(this->hSortPoints, this->hSortPoints);
    // sort by key
    Algorithm::SortByKey(hBucketIds, this->hSortPoints);
    return hBucketIds;
}

void PointLocator::formatBucketIds(ArrayHandle<dax::Id> hBucketIds)
{
    // use the unique operator to find out the unique bucket ids that have points
    ArrayHandle<dax::Id> hUniqueBucketIds;
    Algorithm::Copy(hBucketIds, hUniqueBucketIds);
    Algorithm::Unique(hUniqueBucketIds);
    // use lowerbound to find the starting positions of the ids 
    ArrayHandle<dax::Id> hPointStartIds;
    Algorithm::LowerBounds(hBucketIds, hUniqueBucketIds, // inputs
                           hPointStartIds); // outputs

    // calculate the count of points in each bucket
    // apply the functor to find the counts
    ArrayHandle<int> hBucketPointCounts;
    dax::Id numUniqueKeys = hUniqueBucketIds.GetNumberOfValues();
    Offset2CountFunctor offset2Count(
            hPointStartIds.PrepareForInput(),
            hBucketPointCounts.PrepareForOutput(numUniqueKeys),
            numUniqueKeys - 1,
            hBucketIds.GetNumberOfValues());
    Algorithm::Schedule(offset2Count, numUniqueKeys);

    // allocate memory for the point start array and point count array
    // initialize point start array to -1 for emptyness
    ArrayHandleConstant<dax::Id> hPointStartInit(-1, bucketCount());
    Algorithm::Copy(hPointStartInit, this->hPointStarts);
    // initialize point count array to 0 for emptyness
    ArrayHandleConstant<int> hPointCountInit( 0, bucketCount());
    Algorithm::Copy(hPointCountInit, this->hPointCounts);
    // parallel using the hUniqueBucketIds to input the
    // hPointStartIds and hBucketPointCounts into
    // hPointStart and hPointCount
    Explicit2ImplicitIndex<dax::Id> convertPointStarts(
            hUniqueBucketIds.PrepareForInput(),
            hPointStartIds.PrepareForInput(),
            this->hPointStarts.PrepareForOutput(bucketCount()));
    Algorithm::Schedule(convertPointStarts, numUniqueKeys);
    Explicit2ImplicitIndex<int> convertPointCounts(
            hUniqueBucketIds.PrepareForInput(),
            hBucketPointCounts.PrepareForInput(),
            this->hPointCounts.PrepareForOutput(bucketCount()));
    Algorithm::Schedule(convertPointCounts, numUniqueKeys);
}

dax::Id PointLocator::binPoint(const dax::Vector3& point) const
{
    return MapPointToBucket(this->grid()).MapToFlatIndex(point);
}

dax::Vector3 PointLocator::bounds() const
{
    return this->Bounds;
}

dax::Id3 PointLocator::dimensions() const
{
    // if automatic, calculate divisions base on pointsPerBucket
    if (this->automatic)
    {
        // equation: div.x = pow(N/pointsPerBucket, 1/3)
        dax::Id level = pow(hSortPoints.GetNumberOfValues() / 3, 1.f/3.f) + 0.5;
        return dax::Id3(level, level, level);
    }
    // if manual, use user specified divisions
    return this->Dimensions;
}

int PointLocator::bucketCount() const
{
    return this->grid().GetNumberOfCells();
}

UniformGrid<> PointLocator::grid() const
{
    dax::Vector3 bound = bounds();
    dax::Id3 dim = dimensions();
    UniformGrid<> ret;
    ret.SetOrigin(dax::make_Vector3(0.0, 0.0, 0.0));
    ret.SetSpacing(dax::make_Vector3(bound[0] / dim[0],
                                     bound[1] / dim[1],
                                     bound[2] / dim[2]));
    ret.SetExtent(dax::make_Id3(0, 0, 0), dim);
    return ret;
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
