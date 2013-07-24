#define BOOST_SP_DISABLE_THREADS

#include "DaxLocator.h"

#include <iostream>
#include <iomanip>

#include <dax/cont/ArrayHandleConstant.h>

#include "BinPoints.h"

typedef dax::cont::internal::DeviceAdapterAlgorithm<DAX_DEFAULT_DEVICE_ADAPTER_TAG> Algorithm;

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

void DaxLocator::setSpacing(float x, float y, float z)
{
    this->grid.SetSpacing(dax::make_Vector3(x, y, z));
}

void DaxLocator::setExtent(int xmin, int xmax,
                           int ymin, int ymax,
                           int zmin, int zmax)
{
    this->grid.SetOrigin(dax::make_Vector3(float(xmin), float(ymin), float(zmin)));
    this->grid.SetExtent(dax::make_Id3(xmin, ymin, zmin),
                         dax::make_Id3(xmax, ymax, zmax));
}

void DaxLocator::setPoints(const std::vector<dax::Vector3>& points)
{
    this->hSortPoints = make_ArrayHandle(points);
}

void DaxLocator::build()
{
    // Step 1: map points to bin
    ArrayHandle<dax::Id> hOriBucketIds = mapPoints2Bin();
    // Step 2: sort points according to bucket ids
    ArrayHandle<dax::Id> hBucketIds = sortPoints(hOriBucketIds);
    // Step 3: turn the bucketIds array into offset and count format
    formatBucketIds(hBucketIds);
}

std::vector<dax::Vector3> DaxLocator::getSortPoints() const
{
    std::vector<dax::Vector3> sortPoints(hSortPoints.GetNumberOfValues());
    hSortPoints.CopyInto(sortPoints.begin());
    return sortPoints;
}

std::vector<dax::Id> DaxLocator::getPointStarts() const
{
    std::vector<dax::Id> pointStarts(hPointStarts.GetNumberOfValues());
    hPointStarts.CopyInto(pointStarts.begin());
    return pointStarts;
}

std::vector<int> DaxLocator::getPointCounts() const
{
    std::vector<int> pointCounts(hPointCounts.GetNumberOfValues());
    hPointCounts.CopyInto(pointCounts.begin());
    return pointCounts;
}

dax::Id DaxLocator::locatePoint(const dax::Vector3& point) const
{
    // find the bucket id that the point belongs to
    dax::Id id = binPoint(point);
    return id;
}

std::vector<dax::Vector3> DaxLocator::getBucketPoints(const dax::Id& bucketId) const
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

ExecLocator DaxLocator::prepareExecutionObject() const
{
    ExecLocator ret;
    ret.setOrigin(origin());
    ret.setSpacing(spacing());
    ret.setExtent(extent());
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

ArrayHandle<dax::Id> DaxLocator::mapPoints2Bin()
{
    // use a worklet to find out which bin each point belongs to
    // results are stored in this->hOriBucketIds
    ArrayHandle<dax::Id> hOriBucketIds;
    Scheduler<> scheduler;
    scheduler.Invoke(dax::worklet::BinPoints(),
                     this->hSortPoints,
                     origin(),
                     spacing(),
                     extent(),
                     hOriBucketIds);
    return hOriBucketIds;
}

ArrayHandle<dax::Id> DaxLocator::sortPoints(ArrayHandle<dax::Id> hOriBucketIds)
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

void DaxLocator::formatBucketIds(ArrayHandle<dax::Id> hBucketIds)
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
    int cellCount = this->grid.GetNumberOfCells();
    // initialize point start array to -1 for emptyness
    ArrayHandleConstant<dax::Id> hPointStartInit(-1, cellCount);
    Algorithm::Copy(hPointStartInit, this->hPointStarts);
    // initialize point count array to 0 for emptyness
    ArrayHandleConstant<int> hPointCountInit( 0, cellCount);
    Algorithm::Copy(hPointCountInit, this->hPointCounts);
    // parallel using the hUniqueBucketIds to input the
    // hPointStartIds and hBucketPointCounts into
    // hPointStart and hPointCount
    Coarse2ImplicitFunctor coarse2Implicit(
            hUniqueBucketIds.PrepareForInput(),
            hPointStartIds.PrepareForInput(),
            hBucketPointCounts.PrepareForInput(),
            this->hPointStarts.PrepareForOutput(cellCount),
            this->hPointCounts.PrepareForOutput(cellCount));
    Algorithm::Schedule(coarse2Implicit, numUniqueKeys);
}

dax::Vector3 DaxLocator::origin() const
{
    return grid.GetOrigin();
}

dax::Vector3 DaxLocator::spacing() const
{
    return grid.GetSpacing();
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

dax::Id DaxLocator::binPoint(const dax::Vector3& point) const
{
    int resolution[3] = {extent().Max[0] - extent().Min[0],
                         extent().Max[1] - extent().Min[1],
                         extent().Max[2] - extent().Min[2]};
    // compute the point coordinate within the grid
    dax::Vector3 coord(point[0] - origin()[0],
                       point[1] - origin()[1],
                       point[2] - origin()[2]);
    // which bucket the point belongs
    dax::Id xid, yid, zid;
    xid = dax::math::Floor(coord[0] / spacing()[0]);
    yid = dax::math::Floor(coord[1] / spacing()[1]);
    zid = dax::math::Floor(coord[2] / spacing()[2]);
    if (fabs(spacing()[2]) < 0.0001)
        zid = 0;
    return xid + yid * resolution[0] + zid * resolution[0] * resolution[2];
}
