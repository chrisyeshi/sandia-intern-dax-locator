#include <iostream>
#include <cassert>

#include <dax/Types.h>

#include "PointLocator.h"

int main(void)
{
    // fake points, we just need to number of points
    std::vector<dax::Vector3> pts128(128);
    std::vector<dax::Vector3> pts129(129);

    // PointLocator to automatically calculate the divisions
    PointLocator locator128;
    locator128.setPoints(pts128);
    dax::Id3 divs128 = locator128.getDivs();
    std::cout << "Divs 128: " << divs128[0] << ", "
              << divs128[1] << ", " << divs128[2] << std::endl;
    assert(divs128[0] == 3 && divs128[1] == 3 && divs128[2] == 3);

    PointLocator locator129;
    locator129.setPoints(pts129);
    dax::Id3 divs129 = locator129.getDivs();
    std::cout << "Divs 129: " << divs129[0] << ", "
              << divs129[1] << ", " << divs129[2] << std::endl;
    assert(divs129[0] == 4 && divs129[1] == 4 && divs129[2] == 4);

    return 0;
}
