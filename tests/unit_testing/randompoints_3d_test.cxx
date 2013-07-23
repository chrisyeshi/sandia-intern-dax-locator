#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <cmath>

#include "../RandomPoints3D.h"
#include "stdnormal_test.h"

#define equal_point(pt1, pt2) assert(fabs(pt1.x() - pt2.x()) < 0.0001 && fabs(pt1.y() - pt2.y()) < 0.0001);

int main(void)
{
    int count = 10000;

    // generate the points
    RandomPoints3D random;
    random.setExtent(0, 100, 0, 100, 0, 100);
    random.setPointCount(count);
    random.generate();
    std::vector<Point3D> pts1 = random.getPoints();
    assert(pts1.size() == count);

    // extrace the 2d points
    std::vector<Point2D> pts2d(count);
    for (unsigned int i = 0; i < count; ++i)
    {
        Point3D pt3d = pts1[i];
        Point2D pt2d(pt3d[0], pt3d[1]);
        pts2d[i] = pt2d;
    }

    // test
    float exp_mean[2] = {50.f, 50.f};
    float exp_std[2] = {16.66, 16.66};
    testStdNormal(pts2d, exp_mean, exp_std);
    
    return 0;
}
