#include <iostream>

#include <dax/cont/UniformGrid.h>

using namespace dax::cont;

int main(void)
{
    // create uniform grid
    UniformGrid<> grid;
    grid.SetOrigin(dax::make_Vector3(0.0, 0.0, 0.0));
    grid.SetSpacing(dax::make_Vector3(10.0, 10.0, 10.0));
    grid.SetExtent(dax::make_Id3(0, 0, 0), dax::make_Id3(99, 99, 99));

    // test the grid point coordinate
    dax::Vector3 coord = grid.ComputePointCoordinates(dax::make_Id3(1, 1, 1));
    std::cout << coord[0] << ", " << coord[1] << ", " << coord[2] << std::endl;
    assert(coord[0] == 10 && coord[1] == 10 && coord[2] == 10);

    return 0;
}
