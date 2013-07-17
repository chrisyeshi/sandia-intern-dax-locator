#include "Point2D.h"

#include "cassert"

Point2D::Point2D()
{
    init();
}

Point2D::Point2D(float x, float y)
{
    init();
    coord[0] = x;
    coord[1] = y;
}

Point2D::Point2D(float xy[2])
{
    init();
    coord[0] = xy[0];
    coord[1] = xy[1];
}

Point2D::Point2D(std::vector<float> xy)
{
    init();
    assert(xy.size() >= 2);
    coord[0] = xy[0];
    coord[1] = xy[1];
}

Point2D::~Point2D()
{
}

void Point2D::init()
{
    coord.resize(2);
    coord[0] = coord[1] = 0.f;
}
