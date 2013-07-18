#include <iostream>

#include <dax/cont/UniformGrid.h>
#include <dax/Types.h>

using namespace dax::cont;

int main(void)
{
    UniformGrid<> grid;
    grid.SetOrigin(dax::Vector3(0.0, 0.0, 0.0));
    grid.SetSpacing(dax::Vector3(1.0, 1.0, 0.0));
    grid.SetExtent(dax::Id3(0, 0, 0), dax::Id3(3, 3, 1));

    std::cout << "Cell Count: " << grid.GetNumberOfCells() << std::endl;

    return 0;
}
