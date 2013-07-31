#include "CellLocator.h"

#include <dax/cont/internal/DeviceAdapterAlgorithm.h>
#include <dax/cont/DeviceAdapter.h>
#include <dax/math/Compare.h>
#include <dax/math/Precision.h>
#include <dax/cont/ArrayHandleConstant.h>

#include "BinTriangle.h"
#include "CountOverlappingBuckets.h"
#include "Offset2CountFunctor.h"
#include "Coarse2ImplicitFunctor.h"

typedef dax::cont::internal::DeviceAdapterAlgorithm<DAX_DEFAULT_DEVICE_ADAPTER_TAG>
    Algorithm;

///////////////////////////////////////////////////////////////////////////
//
//
//
//
// Public
//
//
//
//
///////////////////////////////////////////////////////////////////////////

CellLocator::CellLocator()
{
}

CellLocator::~CellLocator()
{
}

void CellLocator::setDivisions(int x, int y, int z)
{
    this->divisions[0] = x;
    this->divisions[1] = y;
    this->divisions[2] = z;
}

void CellLocator::setExtent(int xmin, int xmax,
                            int ymin, int ymax,
                            int zmin, int zmax)
{
    this->extent.Min = dax::make_Id3(xmin, ymin, zmin);
    this->extent.Max = dax::make_Id3(xmax, ymax, zmax);
}

void CellLocator::setPoints(const std::vector<dax::Vector3>& points)
{
    hPoints = make_ArrayHandle(points);
}

void CellLocator::setConnections(const std::vector<dax::Id>& connections)
{
    hConnections = make_ArrayHandle(connections);
}

void CellLocator::build()
{
    // construction of the uniform grid search structure
    // 1. find how many buckets each cell overlaps, declare memory
    CountOverlappingBuckets countOverlappingBuckets(
            hPoints.PrepareForInput(),
            hConnections.PrepareForInput(),
            extent,
            divisions,
            hOverlapBucketCounts.PrepareForOutput(cellCount()));
    Algorithm::Schedule(countOverlappingBuckets, cellCount());
    // scan the hOverlapBucketCounts array to obtain the total length of the map,
    // also the sub sums can be used as index when mapping the cells.
    this->totalBucketCount = Algorithm::ScanExclusive(hOverlapBucketCounts,
                                                      hScanBucketCounts);

    // 2. map each cell to its overlapping buckets
    BinTriangles binTriangles(
            hPoints.PrepareForInput(),
            hConnections.PrepareForInput(),
            hScanBucketCounts.PrepareForInput(),
            extent,
            divisions,
            hCellIds.PrepareForOutput(this->totalBucketCount),
            hBucketIds.PrepareForOutput(this->totalBucketCount));
    Algorithm::Schedule(binTriangles, cellCount());

    // 3. sort by bucket ids
    Algorithm::Copy(hCellIds, hSortCellIds);
    Algorithm::Copy(hBucketIds, hSortBucketIds);
    Algorithm::SortByKey(hSortBucketIds, hSortCellIds);

    // 4. reduce to offset-count format
    // unique and lowerbound, then translate to count
    Algorithm::Copy(hSortBucketIds, hUniqueBucketIds);
    Algorithm::Unique(hUniqueBucketIds);
    Algorithm::LowerBounds(hSortBucketIds, hUniqueBucketIds, hCellStartIds);
    int numUniqueKeys = hUniqueBucketIds.GetNumberOfValues();
    Offset2CountFunctor offset2Count(
            hCellStartIds.PrepareForInput(),
            hBucketCellCounts.PrepareForOutput(numUniqueKeys),
            numUniqueKeys - 1,
            hSortBucketIds.GetNumberOfValues());
    Algorithm::Schedule(offset2Count, numUniqueKeys);

    // expand to implicit indexing
    ArrayHandleConstant<dax::Id> hCellStartInit(-1, bucketCount());
    Algorithm::Copy(hCellStartInit, hCellStarts);
    ArrayHandleConstant<int> hCellCountInit(0, bucketCount());
    Algorithm::Copy(hCellCountInit, hCellCounts);
    Coarse2ImplicitFunctor coarse2Implicit(
            hUniqueBucketIds.PrepareForInput(),
            hCellStartIds.PrepareForInput(),
            hBucketCellCounts.PrepareForInput(),
            hCellStarts.PrepareForOutput(bucketCount()),
            hCellCounts.PrepareForOutput(bucketCount()));
    Algorithm::Schedule(coarse2Implicit, numUniqueKeys);
}

std::vector<int> CellLocator::getOverlapBucketCounts() const
{
    std::vector<int> ret(hOverlapBucketCounts.GetNumberOfValues());
    hOverlapBucketCounts.CopyInto(ret.begin());
    return ret;
}

std::vector<int> CellLocator::getScanBucketCounts() const
{
    std::vector<int> ret(hScanBucketCounts.GetNumberOfValues());
    hScanBucketCounts.CopyInto(ret.begin());
    return ret;
}

std::vector<dax::Id> CellLocator::getCellIds() const
{
    std::vector<dax::Id> ret(hCellIds.GetNumberOfValues());
    hCellIds.CopyInto(ret.begin());
    return ret;
}

std::vector<dax::Id> CellLocator::getOverlapBucketIds() const
{
    std::vector<dax::Id> ret(hBucketIds.GetNumberOfValues());
    hBucketIds.CopyInto(ret.begin());
    return ret;
}

std::vector<dax::Id> CellLocator::getSortCellIds() const
{
    std::vector<dax::Id> ret(hSortCellIds.GetNumberOfValues());
    hSortCellIds.CopyInto(ret.begin());
    return ret;
}

std::vector<dax::Id> CellLocator::getSortBucketIds() const
{
    std::vector<dax::Id> ret(hSortBucketIds.GetNumberOfValues());
    hSortBucketIds.CopyInto(ret.begin());
    return ret;
}

std::vector<dax::Id> CellLocator::getUniqueBucketIds() const
{
    std::vector<dax::Id> ret(hUniqueBucketIds.GetNumberOfValues());
    hUniqueBucketIds.CopyInto(ret.begin());
    return ret;
}

std::vector<dax::Id> CellLocator::getCellStartIds() const
{
    std::vector<dax::Id> ret(hCellStartIds.GetNumberOfValues());
    hCellStartIds.CopyInto(ret.begin());
    return ret;
}

std::vector<dax::Id> CellLocator::getBucketCellCounts() const
{
    std::vector<dax::Id> ret(hBucketCellCounts.GetNumberOfValues());
    hBucketCellCounts.CopyInto(ret.begin());
    return ret;
}

std::vector<dax::Id> CellLocator::getCellStarts() const
{
    std::vector<dax::Id> ret(hCellStarts.GetNumberOfValues());
    hCellStarts.CopyInto(ret.begin());
    return ret;
}

std::vector<dax::Id> CellLocator::getCellCounts() const
{
    std::vector<dax::Id> ret(hCellCounts.GetNumberOfValues());
    hCellCounts.CopyInto(ret.begin());
    return ret;
}

///////////////////////////////////////////////////////////////////////////
//
//
//
//
// Protected
//
//
//
//
///////////////////////////////////////////////////////////////////////////

int CellLocator::cellCount() const
{
    return this->hConnections.GetNumberOfValues() / this->verticesPerCell;
}

int CellLocator::bucketCount() const
{
    return divisions[0] * divisions[1] * divisions[2];
}
