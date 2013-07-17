#ifndef __POINT2D_H__
#define __POINT2D_H__

#include <vector>

class Point2D
{
public:
    Point2D();
    Point2D(float x, float y);
    Point2D(float xy[2]);
    Point2D(std::vector<float> xy);
    virtual ~Point2D();

    void x(const float& x) { coord[0] = x; }
    void y(const float& y) { coord[1] = y; }
    float& x() { return coord[0]; }
    float& y() { return coord[1]; }
    float x() const { return coord[0]; }
    float y() const { return coord[1]; }
    float operator[](int index) const { return coord[index]; }
    float& operator[](int index) { return coord[index]; }

protected:
    std::vector<float> coord;

private:
    void init();
};

#endif // __POINT2D_H__
