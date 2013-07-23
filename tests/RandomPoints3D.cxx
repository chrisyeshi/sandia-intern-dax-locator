#include "RandomPoints3D.h"

#include <cmath>
#include <cassert>

RandomPoints3D::RandomPoints3D()
{
    extent.resize(6);
    extent[0] = extent[2] = extent[4] = -3;
    extent[1] = extent[3] = extent[5] = 3;
}

RandomPoints3D::~RandomPoints3D()
{
}

void RandomPoints3D::setPointCount(int count)
{
    points.resize(count);
}

void RandomPoints3D::setExtent(int xmin, int xmax,
                               int ymin, int ymax,
                               int zmin, int zmax)
{
    extent[0] = xmin;
    extent[1] = xmax;
    extent[2] = ymin;
    extent[3] = ymax;
    extent[4] = zmin;
    extent[5] = zmax;
}

void RandomPoints3D::generate()
{
    // don't know how to generate normal distribution on 3d points,
    // so the Z is uniform distributed.
    // TODO: 3d normal distribution
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        // Box-Muller transform
        float U = drand48();
        float V = drand48();
        float X = sqrt(-2.f * log(U)) * cos(2.f * M_PI * V);
        float Y = sqrt(-2.f * log(U)) * sin(2.f * M_PI * V);
        // X and Y are standard normal distribution
        // scale them to the according size
        float sx = (X + 3.f) / 6.f
                 * float(extent[1] - extent[0]) + float(extent[0]);
        float sy = (Y + 3.f) / 6.f
                 * float(extent[3] - extent[2]) + float(extent[2]);
        // clamp the value to be within extent
        if (sx < float(extent[0])) sx = float(extent[0]);
        if (sx >= float(extent[1])) sx = float(extent[1]) - 0.0001;
        if (sy < float(extent[2])) sy = float(extent[2]);
        if (sy >= float(extent[3])) sy = float(extent[3]) - 0.0001;
        // Z is uniform
        float Z = drand48();
        float sz = Z * float(extent[5] - extent[4]) + float(extent[4]);
        // Use sx, sy, and sz as point coordinate
        Point3D pt(sx, sy, sz);
        points[i] = pt;
    }
}
