#define BOOST_SP_DISABLE_THREADS

#include "CellLocator.h"

#include <dax/cont/internal/DeviceAdapterAlgorithm.h>
#include <dax/cont/DeviceAdapter.h>
#include <dax/math/Compare.h>
#include <dax/math/Precision.h>
#include <dax/cont/ArrayHandleConstant.h>

#include "BinTriangle.h"
#include "CountOverlapBuckets.h"
#include "Offset2CountFunctor.h"
#include "Explicit2ImplicitIndex.h"

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

void CellLocator::setDimensions(int x, int y, int z)
{
    dax::Vector3 bound = bounds();
    this->grid.SetExtent(dax::make_Id3(0, 0, 0), dax::make_Id3(x, y, z));
    this->setBounds(bound[0], bound[1], bound[2]);
}

void CellLocator::setBounds(float x, float y, float z)
{
    dax::Id3 dimensions = dax::extentCellDimensions(this->grid.GetExtent());
    this->grid.SetSpacing(dax::make_Vector3(x / dimensions[0],
                                            y / dimensions[1],
                                            z / dimensions[2]));
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
    CountOverlapBuckets countOverlapBuckets(
            hPoints.PrepareForInput(),
            hConnections.PrepareForInput(),
            grid.PrepareForInput(),
            hOverlapBucketCounts.PrepareForOutput(cellCount()));
    Algorithm::Schedule(countOverlapBuckets, cellCount());
    // scan the hOverlapBucketCounts array to obtain the total length of the map,
    // also the sub sums can be used as index when mapping the cells.
    this->totalBucketCount = Algorithm::ScanExclusive(hOverlapBucketCounts,
                                                      hScanBucketCounts);

    // 2. map each cell to its overlapping buckets
    BinTriangles binTriangles(
            hPoints.PrepareForInput(),
            hConnections.PrepareForInput(),
            hScanBucketCounts.PrepareForInput(),
            grid.PrepareForInput(),
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
    Explicit2ImplicitIndex<dax::Id> convertCellStarts(
            hUniqueBucketIds.PrepareForInput(),
            hCellStartIds.PrepareForInput(),
            this->hCellStarts.PrepareForOutput(bucketCount()));
    Algorithm::Schedule(convertCellStarts, numUniqueKeys);
    Explicit2ImplicitIndex<int> convertCellCounts(
            hUniqueBucketIds.PrepareForInput(),
            hBucketCellCounts.PrepareForInput(),
            this->hCellCounts.PrepareForOutput(bucketCount()));
    Algorithm::Schedule(convertCellCounts, numUniqueKeys);
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
    return this->grid.GetNumberOfCells();
}

dax::Vector3 CellLocator::bounds() const
{
    dax::Vector3 spacing = this->grid.GetSpacing();
    return dax::make_Vector3(spacing[0] * dimensions()[0],
                             spacing[1] * dimensions()[1],
                             spacing[2] * dimensions()[2]);
}

dax::Id3 CellLocator::dimensions() const
{
    return dax::extentCellDimensions(this->grid.GetExtent());
}
