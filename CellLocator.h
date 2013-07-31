#ifndef __CELLLOCATOR_H__
#define __CELLLOCATOR_H__

#include <vector>

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/cont/Arrayhandle.h>

using namespace dax::cont;

class CellLocator
{
public:
    CellLocator();
    virtual ~CellLocator();

    void setDivisions(int x, int y, int z);
    void setExtent(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax);
    void setPoints(const std::vector<dax::Vector3>& points);
    void setConnections(const std::vector<dax::Id>& connections);
    void build();

    std::vector<int> getOverlapBucketCounts() const;
    std::vector<int> getScanBucketCounts() const;
    std::vector<dax::Id> getCellIds() const;
    std::vector<dax::Id> getOverlapBucketIds() const;
    std::vector<dax::Id> getSortCellIds() const;
    std::vector<dax::Id> getSortBucketIds() const;
    std::vector<dax::Id> getUniqueBucketIds() const;
    std::vector<dax::Id> getCellStartIds() const;
    std::vector<int> getBucketCellCounts() const;
    std::vector<dax::Id> getCellStarts() const;
    std::vector<int> getCellCounts() const;

protected:
    static const int verticesPerCell = 3;

    dax::Id3 divisions;
    dax::Extent3 extent;
    ArrayHandle<dax::Vector3> hPoints;
    ArrayHandle<dax::Id> hConnections;
    ArrayHandle<int> hOverlapBucketCounts;
    int totalBucketCount;
    ArrayHandle<int> hScanBucketCounts;
    ArrayHandle<dax::Id> hCellIds;
    ArrayHandle<dax::Id> hBucketIds;
    ArrayHandle<dax::Id> hSortCellIds;
    ArrayHandle<dax::Id> hSortBucketIds;
    ArrayHandle<dax::Id> hUniqueBucketIds;
    ArrayHandle<dax::Id> hCellStartIds;
    ArrayHandle<int> hBucketCellCounts;
    ArrayHandle<dax::Id> hCellStarts;
    ArrayHandle<int> hCellCounts;

    int cellCount() const;
    int bucketCount() const;

private:
};

#endif //__CELLLOCATOR_H__
