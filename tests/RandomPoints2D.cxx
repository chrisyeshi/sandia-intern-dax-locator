#include "RandomPoints2D.h"

#include <fstream>
#include <cmath>
#include <cassert>

RandomPoints2D::RandomPoints2D()
{
    extent.resize(4);
    extent[0] = extent[2] = -3;
    extent[1] = extent[3] = 2;
}

RandomPoints2D::~RandomPoints2D()
{
}

void RandomPoints2D::setPointCount(int count)
{
    points.resize(count);
}

void RandomPoints2D::setExtent(int xmin, int xmax, int ymin, int ymax)
{
    extent[0] = xmin;
    extent[1] = xmax;
    extent[2] = ymin;
    extent[3] = ymax;
}

void RandomPoints2D::generate()
{
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        // Box-Muller transform
        float U = drand48();
        float V = drand48();
        float X = sqrt(-2.f * log(U)) * cos(2.f * M_PI * V);
        float Y = sqrt(-2.f * log(U)) * sin(2.f * M_PI * V);
        // X and Y are standard normal distribution in [-1,1)
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
        // Use X, Y and point coordinate
        Point2D pt(sx, sy);
        points[i] = pt;
    }
}

void RandomPoints2D::write(const char* path) const
{
    this->write(std::string(path));
}

void RandomPoints2D::write(const std::string& path) const
{
    std::ofstream fout(path.c_str(), std::ofstream::binary);

    // fisrt 4 bytes is for the count of points
    int count = points.size();
    fout.write(reinterpret_cast<char *>(&count), sizeof(count));

    // structure the points to X array and then Y array
    float xarray[count], yarray[count];
    for (unsigned int i = 0; i < count; ++i)
    {
        xarray[i] = points[i].x();
        yarray[i] = points[i].y();
    }

    // then write the points, X array then Y array
    fout.write(reinterpret_cast<char *>(xarray), count * sizeof(float));
    fout.write(reinterpret_cast<char *>(yarray), count * sizeof(float));

    fout.close();
}

bool RandomPoints2D::read(const char *path)
{
    return this->read(std::string(path));
}

bool RandomPoints2D::read(const std::string& path)
{
    std::ifstream fin(path.c_str(), std::ifstream::binary);
    if (!fin.good())
        return false;
    
    // first read the first 4 bytes, it is the number of points
    int count = -1;
    fin.read(reinterpret_cast<char *>(&count), sizeof(int));

    // read the points into X array and Y array
    float xarray[count], yarray[count];
    fin.read(reinterpret_cast<char *>(xarray), count * sizeof(float));
    fin.read(reinterpret_cast<char *>(yarray), count * sizeof(float));

    // structure them into Point2D points
    // allocate space according to the count
    assert(count >= 0);
    points.resize(count);
    for (unsigned int i = 0; i < count; ++i)
        points[i] = Point2D(xarray[i], yarray[i]);

    fin.close();
    return true;
}
