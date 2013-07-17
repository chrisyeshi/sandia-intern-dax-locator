#include "DaxLocator.h"

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

void DaxLocator::setPoints(const std::vector<dax::Vector2>& points)
{
    this->hPoints = make_ArrayHandle(points);
}

void DaxLocator::build()
{
    // create the uniform grid 
    constructUniformGrid();

    // Step 1: map points to bin
    mapPoints2Bin();

    // Step 2: sort points according to cell ids
    sortPoints();

    // Step 3: turn the cellIds array into offset and count format
    // cellIds: [1, 1, 2, 2, 2, 3]
    // ==>
    // offset: [0, 2, 5]
    // count: [2, 3, 1]
    formatCellIds();
}

std::vector<dax::Id> DaxLocator::getOriCellIds() const
{
    std::vector<dax::Id> oriCellIds(hOriCellIds.GetNumberOfValues());
    hOriCellIds.CopyInto(oriCellIds.begin());
    return oriCellIds;
}

std::vector<dax::Id> DaxLocator::getCellIds() const
{
    std::vector<dax::Id> cellIds(hCellIds.GetNumberOfValues());
    hCellIds.CopyInto(cellIds.begin());
    return cellIds;
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

std::vector<dax::Id> DaxLocator::getUniqueCellIds() const
{
    return uniqueCellIds;
}

std::vector<dax::Id> DaxLocator::getPointStartIds() const
{
    return pointStartIds;
}

std::vector<int> DaxLocator::getCellPointCounts() const
{
    return cellPointCounts;
}

dax::Id DaxLocator::locatePoint(const dax::Vector2& point) const
{
    // find the cell id that the point belongs to
    dax::Id id = binPoint(point);
    return id;
}

std::vector<dax::Vector2> DaxLocator::getCellPoints(const dax::Id& cellId) const
{
    // find the points in the same cell
    std::vector<dax::Id>::const_iterator startItr
        = std::find(this->uniqueCellIds.begin(), this->uniqueCellIds.end(), cellId);
    // make sure the cell contains at least 1 point
    // if not, return an empty array
    if (startItr == this->uniqueCellIds.end())
        return std::vector<dax::Vector2>();
    // if yes, get the index for the point arrays
    dax::Id uniqueIndex = std::distance(this->uniqueCellIds.begin(), startItr);
    // staring point id and number of points in this cell
    dax::Id start = this->pointStartIds[uniqueIndex];
    int count = this->cellPointCounts[uniqueIndex];
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

void DaxLocator::constructUniformGrid()
{
    // temporary it's a 3x3 uniform grid with each cell 1x1.
    // expect to use some algorithm to calculate the grid parameters later.
    grid.SetOrigin(dax::make_Vector3(0.0, 0.0, 0.0));
    grid.SetOrigin(dax::make_Vector3(1.0, 1.0, 1.0));
    grid.SetExtent(dax::make_Id3(0, 0, 0), dax::make_Id3(2, 2, 0));
}

void DaxLocator::mapPoints2Bin()
{
    // use a worklet to find out which bin each point belongs to
    // results are stored in this->hOriCellIds
    Scheduler<> scheduler;
    scheduler.Invoke(dax::worklet::BinPoints(),
                     hPoints,
                     origin(),
                     spacing(),
                     extent(),
                     this->hOriCellIds);
}

void DaxLocator::sortPoints()
{
    // sort the point array according to the cellIds array
    // use the sorting functions provided by dax
    // copy into the new variables
    Algorithm::Copy(this->hOriCellIds, this->hCellIds);
    Algorithm::Copy(this->hPoints, this->hSortPoints);
    // sort by key
    Algorithm::SortByKey(this->hCellIds, this->hSortPoints);
}

void DaxLocator::formatCellIds()
{
    // apply a unique operator to the cellIds array
    std::vector<dax::Id> cellIds = getCellIds();
    assert(!cellIds.empty());
    this->uniqueCellIds.push_back(cellIds[0]);
    this->pointStartIds.push_back(0);
    this->cellPointCounts.push_back(1);
    for (unsigned int i = 1; i < cellIds.size(); ++i)
    {
        // index to the last element in uniqueCellIds
        int uniqueIndex = uniqueCellIds.size() - 1;
        // if the current cellIds[i] matches the last uniqueCellId 
        // then increment the cellPointCounts
        // else it becomes the next uniqueCellId 
        if (cellIds[i] == uniqueCellIds[uniqueIndex])
        {
            this->cellPointCounts[uniqueIndex]++;
        } else
        {
            this->uniqueCellIds.push_back(cellIds[i]);
            this->pointStartIds.push_back(i);
            this->cellPointCounts.push_back(1);
        }
    }
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
    int resolution[2] = {extent().Max[0] - extent().Min[0] + 1,
                         extent().Max[1] - extent().Min[0] + 1};
    // compute the point coordinate within the grid
    dax::Vector2 coord(point[0] - origin()[0], point[1] - origin()[1]);
    // which cell the point belongs
    dax::Id xid, yid;
    xid = dax::math::Floor(coord[0] / spacing()[0]);
    yid = dax::math::Floor(coord[1] / spacing()[1]);
    return xid + yid * resolution[0];
}
