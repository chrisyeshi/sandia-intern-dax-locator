#ifndef __RANDOMPOINTS3D_H__
#define __RANDOMPOINTS3D_H__

#include <vector>

#include "../Point3D.h"

class RandomPoints3D
{
public:
    RandomPoints3D();
    virtual ~RandomPoints3D();

    void setPointCount(int count);
    void setExtent(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax);
    void generate();
    const std::vector<Point3D>& getPoints() const { return points; }

protected:
    std::vector<int> extent;
    std::vector<Point3D> points;

private:
};

#endif //__RANDOMPOINTS3D_H__
