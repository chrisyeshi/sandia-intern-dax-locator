#include "Point3D.h"

#include "Point2D.h"

#include <cassert>

Point3D::Point3D()
{
    init();
}

Point3D::Point3D(float x, float y, float z)
{
    init();
    coord[0] = x;
    coord[1] = y;
    coord[2] = z;
}

Point3D::Point3D(float xyz[3])
{
    init();
    coord[0] = xyz[0];
    coord[1] = xyz[1];
    coord[2] = xyz[2];
}

Point3D::Point3D(std::vector<float> xyz)
{
    init();
    assert(xyz.size() >= 3);
    coord[0] = xyz[0];
    coord[1] = xyz[1];
    coord[2] = xyz[2];
}

Point3D::Point3D(const Point2D& pt2d)
{
    init();
    coord[0] = pt2d[0];
    coord[1] = pt2d[1];
}

Point3D::~Point3D()
{
}

void Point3D::init()
{
    coord.resize(3);
    coord[0] = coord[1] = coord[2] = 0.f;
}
