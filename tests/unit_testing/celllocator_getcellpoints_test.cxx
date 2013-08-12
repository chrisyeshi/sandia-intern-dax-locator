#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include <dax/CellTag.h>
#include <dax/CellTraits.h>
#include <dax/cont/testing/TestingGridGenerator.h>

#include "CellLocator.h"

void comparePoint(dax::Vector3 point, dax::Vector3 correct)
{
    for (int i = 0; i < 3; ++i)
    {
        std::cout << point[i] << " :::: " << correct[i] << std::endl;
        assert(fabs(point[i] - correct[i]) < 0.0001);
    }
    std::cout << std::endl;
}

int main(void)
{
    typedef dax::CellTagHexahedron CellTag;
    typedef dax::cont::UnstructuredGrid<CellTag> GridType;

    dax::cont::testing::TestGrid<GridType> gridGen(3);
    GridType grid = gridGen.GetRealGrid();

    CellLocator<CellTag> locator;
    locator.setAutomatic(false);
    locator.setDimensions(2, 2, 2);
    locator.setBounds(2.01, 2.01, 2.01);
    locator.setPoints(grid.GetPointCoordinates());
    locator.setConnections(grid.GetCellConnections());
    locator.build();

    std::vector<dax::Id> connections = locator.getConnections();
    std::cout << "Connections: ";
    for (unsigned int i = 0; i < connections.size(); ++i)
        std::cout << connections[i] << ", ";
    std::cout << std::endl;

    // locate cell
    dax::Id cellId = locator.locateCell(dax::make_Vector3(1.5, 1.5, 1.5));
    assert(cellId == 7);

    std::vector<dax::Id> bucketCells = locator.getBucketCells(cellId);
    assert(bucketCells.size() == 1);
    assert(bucketCells[0] == 7);

    std::vector<dax::Vector3> cellPoints = locator.getCellPoints(bucketCells[0]);
    assert(cellPoints.size() == dax::CellTraits<CellTag>::NUM_VERTICES);
    comparePoint(cellPoints[0], dax::make_Vector3(1,1,1));
    comparePoint(cellPoints[1], dax::make_Vector3(2,1,1));
    comparePoint(cellPoints[2], dax::make_Vector3(2,2,1));
    comparePoint(cellPoints[3], dax::make_Vector3(1,2,1));
    comparePoint(cellPoints[4], dax::make_Vector3(1,1,2));
    comparePoint(cellPoints[5], dax::make_Vector3(2,1,2));
    comparePoint(cellPoints[6], dax::make_Vector3(2,2,2));
    comparePoint(cellPoints[7], dax::make_Vector3(1,2,2));

    return 0;
}
