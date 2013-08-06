#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <cassert>

#include <dax/Types.h>

#include "CellLocator.h"

using namespace dax::cont;

// main
int main(void)
{
    // 3D situation
    // points and connections
    std::vector<dax::Id> connections3d(3 * 200);

    // use CellLocator to automatically calculate the grid parameters
    CellLocator<dax::CellTagTriangle> locator3d;
    locator3d.setAutomatic(true);
    locator3d.setBounds(1, 1, 1);
    locator3d.setConnections(connections3d);

    // check dimensions
    dax::Id3 dim3d = locator3d.getDimensions();
    assert(dim3d[0] == 10 && dim3d[1] == 10 && dim3d[2] == 10);

    // 2D situation
    std::vector<dax::Id> connections2d(3 * 200);
    CellLocator<dax::CellTagTriangle> locator2d;
    locator2d.setAutomatic(true);
    locator2d.setBounds(1, 1, 0);
    locator2d.setConnections(connections2d);
    dax::Id3 dim2d = locator2d.getDimensions();
    assert(dim2d[0] == 10 && dim2d[1] == 10 && dim2d[2] == 1);

    return 0;
}
