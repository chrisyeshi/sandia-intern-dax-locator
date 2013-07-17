#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>
#include <limits>

#include <dax/Types.h>
#include <dax/cont/UniformGrid.h>
#include <dax/math/Precision.h>

#include "Point2D.h"
#include "tests/RandomPoints2D.h"

using namespace dax::cont;

// helper functions
dax::Id binPoint(const dax::Vector2& point,
                 const dax::Vector2& origin,
                 const dax::Vector2& spacing,
                 const dax::Extent3& extent);

// main
int main(void)
{
    // first generate a bunch of random points
    RandomPoints2D random;
    random.setExtent(0, 2, 0, 2);
    random.setPointCount(20);
    random.generate();
    std::vector<Point2D> points = random.getPoints();
    points.push_back(Point2D(0.99, 0.99));

    // put the points into array handle
    // translate Point2D to dax::vector2
    std::vector<dax::Vector2> daxPoints(points.size());
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        Point2D point = points[i];
        dax::Vector2 daxvec(point.x(), point.y());
        daxPoints[i] = daxvec;
    }
    // make_ArrayHandle
    ArrayHandle<dax::Vector2> hPoints = make_ArrayHandle(daxPoints);

    // create the uniform grid
    // temporary it's a 10x10 uniform grid with each cell 10x10.
    // expect to use some algorithm to calculate the grid parameters later.
    UniformGrid<> grid;
    grid.SetOrigin(dax::make_Vector3(0.0, 0.0, 0.0));
    grid.SetSpacing(dax::make_Vector3(1.0, 1.0, 0.0));
    grid.SetExtent(dax::make_Id3(0, 0, 0), dax::make_Id3(2, 2, 0));

    // Step 1:
    // find out which bin each point belongs to
    std::vector<dax::Id> oriCellIds(daxPoints.size());
    for (unsigned int i = 0; i < daxPoints.size(); ++i)
    {
        // inputs
        dax::Vector2 point = daxPoints[i];
        dax::Vector2 origin(grid.GetOrigin()[0], grid.GetOrigin()[1]);
        dax::Vector2 spacing(grid.GetSpacing()[0], grid.GetSpacing()[1]);
        dax::Extent3 extent = grid.GetExtent();
        // find the cell id the point belongs to
        dax::Id id = binPoint(point, origin, spacing, extent);
        // put into the cellIds array
        oriCellIds[i] = id;
    }

    // Step 2:
    // sort the point array and cellid array according to the cellid array
    // dumbest sort ever for now...
    std::vector<dax::Id> cellIds = oriCellIds;
    std::vector<dax::Vector2> sortPoints = daxPoints;
    for (unsigned int i = 0; i < cellIds.size(); ++i)
        for (unsigned int j = i; j < cellIds.size(); ++j)
        {
            dax::Id a = cellIds[i];
            dax::Id b = cellIds[j];
            dax::Vector2 pta = sortPoints[i];
            dax::Vector2 ptb = sortPoints[j];
            if (b < a)
            {
                cellIds[i] = b;
                sortPoints[i] = ptb;
                cellIds[j] = a;
                sortPoints[j] = pta;
            }
        }

    // Step 3:
    // apply a unique operator to the cellIds array
    std::vector<dax::Id> uniqueCellIds;
    std::vector<dax::Id> pointStartIds;
    std::vector<int> cellPointCounts;
    assert(!cellIds.empty());
    uniqueCellIds.push_back(cellIds[0]);
    pointStartIds.push_back(0);
    cellPointCounts.push_back(1);
    for (unsigned int i = 1; i < cellIds.size(); ++i)
    {
        // index to the last element in uniqueCellIds
        int uniqueIndex = uniqueCellIds.size() - 1;
        // if the current cellIds[i] matches the last uniqueCellId 
        // then increment the cellPointCounts
        // else it becomes the next uniqueCellId 
        if (cellIds[i] == uniqueCellIds[uniqueIndex])
        {
            cellPointCounts[uniqueIndex]++;
        } else
        {
            uniqueCellIds.push_back(cellIds[i]);
            pointStartIds.push_back(i);
            cellPointCounts.push_back(1);
        }
    }

    // print
    std::cout.precision(4);
    std::cout << std::setw(10) << "Cells: ";
    for (unsigned int i = 0; i < oriCellIds.size(); ++i)
        std::cout << std::setw(6) << oriCellIds[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Ori X: ";
    for (unsigned int i = 0; i < daxPoints.size(); ++i)
        std::cout << std::setw(6) << daxPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Ori Y: ";
    for (unsigned int i = 0; i < daxPoints.size(); ++i)
        std::cout << std::setw(6) << daxPoints[i][1] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Cells: ";
    for (unsigned int i = 0; i < cellIds.size(); ++i)
        std::cout << std::setw(6) << cellIds[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Sort X: ";
    for (unsigned int i = 0; i < sortPoints.size(); ++i)
        std::cout << std::setw(6) << sortPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Sort Y: ";
    for (unsigned int i = 0; i < sortPoints.size(); ++i)
        std::cout << std::setw(6) << sortPoints[i][1] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Unique: ";
    for (unsigned int i = 0; i < uniqueCellIds.size(); ++i)
        std::cout << std::setw(3) << uniqueCellIds[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Start: ";
    for (unsigned int i = 0; i < pointStartIds.size(); ++i)
        std::cout << std::setw(3) << pointStartIds[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Count: ";
    for (unsigned int i = 0; i < cellPointCounts.size(); ++i)
        std::cout << std::setw(3) << cellPointCounts[i] << ", ";
    std::cout << std::endl;

    // binning a point 
    float x, y;
    x = y = 1.f;
    std::cout << std::setw(10) << "X: " << x << std::endl;
    std::cout << std::setw(10) << "Y: " << y << std::endl;
    // inputs for binPoint
    dax::Vector2 point(x, y);
    dax::Vector2 origin(grid.GetOrigin()[0], grid.GetOrigin()[1]);
    dax::Vector2 spacing(grid.GetSpacing()[0], grid.GetSpacing()[1]);
    dax::Extent3 extent = grid.GetExtent();
    // find the cell id the point belongs to
    dax::Id id = binPoint(point, origin, spacing, extent);
    // find the points in the same cell
    std::vector<dax::Id>::iterator startItr
        = std::find(uniqueCellIds.begin(), uniqueCellIds.end(), id);
    // make sure the cell contains at least 1 point
//    if (startItr == uniqueCellIds.end())
//        continue;
    dax::Id uniqueIndex = std::distance(uniqueCellIds.begin(), startItr);
    // startint point id and number of points in this cell
    dax::Id start = pointStartIds[uniqueIndex];
    int count = cellPointCounts[uniqueIndex];
    std::vector<dax::Vector2> binPoints(count);
    for (unsigned int i = 0; i < count; ++i)
        binPoints[i] = sortPoints[i + start];
    
    // print 
    std::cout << std::setw(10) << "Cell Id: " << id << std::endl;
    std::cout << std::setw(10) << "Pts X: ";
    for (unsigned int i = 0; i < count; ++i)
        std::cout << std::setw(6) << binPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Pts Y: ";
    for (unsigned int i = 0; i < count; ++i)
        std::cout << std::setw(6) << binPoints[i][1] << ", ";
    std::cout << std::endl;

    return 0;
}

dax::Id binPoint(const dax::Vector2& point,
                 const dax::Vector2& origin,
                 const dax::Vector2& spacing,
                 const dax::Extent3& extent)
{
    int resolution[2] = {extent.Max[0] - extent.Min[0] + 1,
                         extent.Max[1] - extent.Min[0] + 1};
    // compute the point coordinate within the grid
    dax::Vector2 coord(point[0] - origin[0], point[1] - origin[1]);
    // which cell the point belongs
    dax::Id xid, yid;
    xid = dax::math::Floor(coord[0] / spacing[0]);
    yid = dax::math::Floor(coord[1] / spacing[1]);
    return xid + yid * resolution[0];
}
