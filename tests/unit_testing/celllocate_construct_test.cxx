#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>
#include <sstream>

#include <dax/CellTag.h>
#include <dax/cont/testing/TestingGridGenerator.h>

#include "CellLocator.h"

template <class CellType>
void test(int multiplier)
{
    const dax::Id DIM = 4;
    typedef dax::cont::UnstructuredGrid<CellType> GridType;

    // generate an unstructured grid
    dax::cont::testing::TestGrid<GridType> gridGen(DIM);
    GridType grid = gridGen.GetRealGrid();

    // CellLocator
    CellLocator<CellType> locator;
    locator.setAutomatic(false);
    locator.setDimensions(2, 2, 2);
    locator.setBounds(3.01, 3.01, 3.01);
    locator.setPoints(grid.GetPointCoordinates());
    locator.setConnections(grid.GetCellConnections());
    locator.build();

    // correct output
    dax::Id starts[] = {0,8,16,24,32,40,48,56};
    int counts[] = {8,8,8,8,8,8,8,8};

    std::vector<dax::Id> cellStarts = locator.getCellStarts();
    std::vector<int> cellCounts = locator.getCellCounts();
    assert(cellStarts.size() == 8);
    assert(cellCounts.size() == 8);
    for (int i = 0; i < 8; ++i)
    {
        std::cout << "Cell Start = " << cellStarts[i]
                  << " :::: "
                  << "Correct Start: " << starts[i] << std::endl;
        assert(cellStarts[i] == starts[i] * multiplier);
        assert(cellCounts[i] == counts[i] * multiplier);
    }
}

// main
int main(void)
{
    test<dax::CellTagTetrahedron>(2);
    test<dax::CellTagHexahedron>(1);
    test<dax::CellTagWedge>(2);

    return 0;
}
