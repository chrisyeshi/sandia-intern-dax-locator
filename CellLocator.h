#ifndef __CELLLOCATOR_H__
#define __CELLLOCATOR_H__

#include <vector>
#include <iostream>
#include <iomanip>

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/CellTag.h>
#include <dax/CellTraits.h>
#include <dax/math/Exp.h>
#include <dax/cont/Arrayhandle.h>
#include <dax/cont/ArrayHandleConstant.h>
#include <dax/cont/UniformGrid.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>

#include "CountOverlapBuckets.h"
#include "Explicit2ImplicitIndex.h"
#include "BinPrimitives.h"
#include "Offset2CountFunctor.h"

using namespace dax::cont;

template <class CellTag>
class CellLocator
{
public:
    typedef dax::cont::internal::DeviceAdapterAlgorithm<DAX_DEFAULT_DEVICE_ADAPTER_TAG> Algorithm;

    CellLocator() {}
    virtual ~CellLocator() {}

    void setAutomatic(bool automatic) { this->Automatic = automatic; }

    void setDimensions(int x, int y, int z)
    {
        this->Dimensions[0] = x;
        this->Dimensions[1] = y;
        this->Dimensions[2] = z;
    }

    void setBounds(float x, float y, float z)
    {
        this->Bounds[0] = x;
        this->Bounds[1] = y;
        this->Bounds[2] = z;
    }

    void setPoints(ArrayHandle<dax::Vector3> hPoints)
    {
        this->hPoints = hPoints;
    }
    void setPoints(const std::vector<dax::Vector3>& points)
    {
        hPoints = make_ArrayHandle(points);
    }

    void setConnections(ArrayHandle<dax::Id> hConnections)
    {
        this->hConnections = hConnections;
    }
    void setConnections(const std::vector<dax::Id>& connections)
    {
        hConnections = make_ArrayHandle(connections);
    }

    void build()
    {
        // temporary variables
        int totalBucketCount;
        ArrayHandle<int> hOverlapBucketCounts;
        ArrayHandle<int> hScanBucketCounts;
        ArrayHandle<dax::Id> hCellIds;
        ArrayHandle<dax::Id> hBucketIds;
        ArrayHandle<dax::Id> hSortBucketIds;
        ArrayHandle<dax::Id> hUniqueBucketIds;
        ArrayHandle<dax::Id> hCellStartIds;
        ArrayHandle<int> hBucketCellCounts;
        // construction of the uniform grid search structure
        // 1. find how many buckets each cell overlaps, declare memory
        CountOverlapBuckets<CellTag> countOverlapBuckets(
                hPoints.PrepareForInput(),
                hConnections.PrepareForInput(),
                grid().PrepareForInput(),
                hOverlapBucketCounts.PrepareForOutput(cellCount()));
        Algorithm::Schedule(countOverlapBuckets, cellCount());
        // scan the hOverlapBucketCounts array to obtain the total length of the map,
        // also the sub sums can be used as index when mapping the cells.
        totalBucketCount = Algorithm::ScanExclusive(hOverlapBucketCounts,
                                                          hScanBucketCounts);

        // 2. map each cell to its overlapping buckets
        BinPrimitives<CellTag> binPrimitives(
                hPoints.PrepareForInput(),
                hConnections.PrepareForInput(),
                hScanBucketCounts.PrepareForInput(),
                grid().PrepareForInput(),
                hCellIds.PrepareForOutput(totalBucketCount),
                hBucketIds.PrepareForOutput(totalBucketCount));
        Algorithm::Schedule(binPrimitives, cellCount());

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
/*
        // print for debug
        // formatting
        std::cout.precision(2);
        std::cout << std::fixed;

        std::vector<int> overlapBucketCounts(hOverlapBucketCounts.GetNumberOfValues());
        hOverlapBucketCounts.CopyInto(overlapBucketCounts.begin());
        std::cout << std::setw(22) << "Overlap Bucket Counts:";
        for (unsigned int i = 0; i < overlapBucketCounts.size(); ++i)
            std::cout << std::setw(3) << overlapBucketCounts[i] << ",";
        std::cout << std::endl;

        std::vector<int> scanBucketCounts(hScanBucketCounts.GetNumberOfValues());
        hScanBucketCounts.CopyInto(scanBucketCounts.begin());
        std::cout << std::setw(22) << "Scan Bucket Counts:";
        for (unsigned int i = 0; i < scanBucketCounts.size(); ++i)
            std::cout << std::setw(3) << scanBucketCounts[i] << ",";
        std::cout << std::endl;

        std::vector<int> cellIds(hCellIds.GetNumberOfValues());
        hCellIds.CopyInto(cellIds.begin());
        std::cout << std::setw(22) << "Cell Ids:";
        for (unsigned int i = 0; i < cellIds.size(); ++i)
            std::cout << std::setw(3) << cellIds[i] << ",";
        std::cout << std::endl;

        std::vector<int> bucketIds(hBucketIds.GetNumberOfValues());
        hBucketIds.CopyInto(bucketIds.begin());
        std::cout << std::setw(22) << "Bucket Ids:";
        for (unsigned int i = 0; i < bucketIds.size(); ++i)
            std::cout << std::setw(3) << bucketIds[i] << ",";
        std::cout << std::endl;

        std::vector<int> sortBucketIds(hSortBucketIds.GetNumberOfValues());
        hSortBucketIds.CopyInto(sortBucketIds.begin());
        std::cout << std::setw(22) << "Sort Bucket Ids:";
        for (unsigned int i = 0; i < sortBucketIds.size(); ++i)
            std::cout << std::setw(3) << sortBucketIds[i] << ",";
        std::cout << std::endl;

        std::vector<int> uniqueBucketIds(hUniqueBucketIds.GetNumberOfValues());
        hUniqueBucketIds.CopyInto(uniqueBucketIds.begin());
        std::cout << std::setw(22) << "Unique Bucket Ids:";
        for (unsigned int i = 0; i < uniqueBucketIds.size(); ++i)
            std::cout << std::setw(3) << uniqueBucketIds[i] << ",";
        std::cout << std::endl;
        */
    }

    std::vector<dax::Id> getSortCellIds() const
    {
        std::vector<dax::Id> ret(hSortCellIds.GetNumberOfValues());
        hSortCellIds.CopyInto(ret.begin());
        return ret;
    }

    std::vector<dax::Id> getCellStarts() const
    {
        std::vector<dax::Id> ret(hCellStarts.GetNumberOfValues());
        hCellStarts.CopyInto(ret.begin());
        return ret;
    }

    std::vector<int> getCellCounts() const
    {
        std::vector<dax::Id> ret(hCellCounts.GetNumberOfValues());
        hCellCounts.CopyInto(ret.begin());
        return ret;
    }

    dax::Id3 getDimensions() const { return dimensions(); }
    UniformGrid<> getGrid() const { return grid(); }

protected:
    // a constant parameter for automatically determining the grid dimensions
    static const int GridDensity = 5;

    bool Automatic;
    dax::Vector3 Bounds;
    dax::Id3 Dimensions;
    ArrayHandle<dax::Vector3> hPoints;
    ArrayHandle<dax::Id> hConnections;
    ArrayHandle<dax::Id> hSortCellIds;
    ArrayHandle<dax::Id> hCellStarts;
    ArrayHandle<int> hCellCounts;

    int cellCount() const
    {
        return this->hConnections.GetNumberOfValues() / this->verticesPerCell();
    }

    int bucketCount() const
    {
        return this->grid().GetNumberOfCells();
    }

    dax::Vector3 bounds() const { return this->Bounds; }

    dax::Id3 dimensions() const
    {
        // if manual mode, return the user specified dimensions
        if (!Automatic)
            return this->Dimensions;
        // if automatic, we calculate the dimensions automatically
        // the equation: Rxyz = dxyz * (lamda * N / V)^(1/3)
        // where Rxyz is the result dimensions
        // , dxyz is the diagonal, V is the volume of the bounding box
        // , and lamda is user specified grid density
        int lamda = this->GridDensity;
        int N = cellCount();
        // if it's the 2d case, we cannot have 0 for the third dimension
        dax::Vector3 v3 = bounds();
        for (int i = 0; i < 3; ++i)
            v3[i] = v3[i] < 0.0001 ? 1.f : v3[i];
        dax::Scalar V = v3[0] * v3[1] * v3[2];
        dax::Scalar P3 = dax::Scalar(lamda * N) / V;
        dax::Scalar P = dax::math::Pow(P3, dax::Scalar(1.0/3.0));
        dax::Vector3 dxyz = bounds();
        dax::Id3 ret = dax::make_Id3(dxyz[0] * P, dxyz[1] * P, dxyz[2] * P);
        // if anyone dimension is 0, we set it to 1
        for (int i = 0; i < 3; ++i)
            ret[i] = ret[i] == 0 ? 1 : ret[i];
        return ret;
    }

    UniformGrid<> grid() const
    {
        UniformGrid<> ret;
        ret.SetOrigin(dax::make_Vector3(0.f, 0.f, 0.f));
        ret.SetSpacing(dax::make_Vector3(bounds()[0] / dimensions()[0],
                                         bounds()[1] / dimensions()[1],
                                         bounds()[2] / dimensions()[2]));
        ret.SetExtent(dax::make_Id3(0, 0, 0), dimensions());
        return ret;
    }

    int verticesPerCell() const
    {
        return dax::CellTraits<CellTag>::NUM_VERTICES;
    }

private:
};

#endif //__CELLLOCATOR_H__
