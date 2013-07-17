#ifndef __DAXLOCATOR_H__
#define __DAXLOCATOR_H__

#include <dax/Types.h>
#include <dax/cont/UniformGrid.h>
#include <dax/math/Precision.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/DeviceAdapter.h>
#include <dax/cont/internal/DeviceAdapterAlgorithm.h>

using namespace dax::cont;

class DaxLocator
{
public:
    DaxLocator();
    virtual ~DaxLocator();

    void setPoints(const std::vector<dax::Vector2>& points);
    void build();

    std::vector<dax::Id> getOriCellIds() const;
    std::vector<dax::Id> getCellIds() const;
    std::vector<dax::Vector2> getPoints() const;
    std::vector<dax::Vector2> getSortPoints() const;
    std::vector<dax::Id> getUniqueCellIds() const;
    std::vector<dax::Id> getPointStartIds() const;
    std::vector<int> getCellPointCounts() const;
    dax::Id locatePoint(const dax::Vector2& point) const;
    std::vector<dax::Vector2> getCellPoints(const dax::Id& cellId) const;

protected:
    ArrayHandle<dax::Vector2> hPoints;
    UniformGrid<> grid;
    ArrayHandle<dax::Id> hOriCellIds;
    ArrayHandle<dax::Id> hCellIds;
    ArrayHandle<dax::Vector2> hSortPoints;
    std::vector<dax::Id> uniqueCellIds;
    std::vector<dax::Id> pointStartIds;
    std::vector<int> cellPointCounts;

    void constructUniformGrid();
    void mapPoints2Bin();
    void sortPoints();
    void formatCellIds();

    dax::Vector2 origin() const;
    dax::Vector2 spacing() const;
    dax::Extent3 extent() const;

private:
    // CPU version of mapping a single point into a bin
    dax::Id binPoint(const dax::Vector2& point) const;
};

#endif //__DAXLOCATOR_H__
