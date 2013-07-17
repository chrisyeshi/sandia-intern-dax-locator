#include "../../Point2D.h"

#include <iostream>
#include <cmath>
#include <cassert>

#define test_point(pt, X, Y) assert(fabs(pt.x() - X) < 0.0001 && fabs(pt.y() - Y) < 0.0001);

int main(void)
{
    Point2D pt;
    test_point(pt, 0.f, 0.f);
    std::cout << 1 << std::endl;

    pt.x(1.f);
    pt.y(2.f);
    test_point(pt, 1.f, 2.f);
    std::cout << 2 << std::endl;

    pt.x() = 3.f;
    pt.y() = 4.f;
    test_point(pt, 3.f, 4.f);
    std::cout << 3 << std::endl;

    pt[0] = 5.f;
    pt[1] = 6.f;
    test_point(pt, 5.f, 6.f);
    std::cout << 4 << std::endl;

    assert(pt[0] == pt.x());
    assert(pt[1] == pt.y());

    return 0;
}
