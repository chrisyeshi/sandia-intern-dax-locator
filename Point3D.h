#ifndef __POINT3D_H__
#define __POINT3D_H__

#include <vector>

class Point2D;

class Point3D
{
public:
    Point3D();
    Point3D(float x, float y, float z);
    Point3D(float xyz[3]);
    Point3D(std::vector<float> xyz);
    Point3D(const Point2D& pt2d);
    virtual ~Point3D();

    void x(const float& x) { coord[0] = x; }
    void y(const float& y) { coord[1] = y; }
    void z(const float& z) { coord[2] = z; }
    float& x() { return coord[0]; }
    float& y() { return coord[1]; }
    float& z() { return coord[2]; }
    float x() const { return coord[0]; }
    float y() const { return coord[1]; }
    float z() const { return coord[2]; }
    float operator[](int index) const { return coord[index]; }
    float& operator[](int index) { return coord[index]; }

protected:
    std::vector<float> coord;

private:
    void init();
};

#endif //__POINT3D_H__
