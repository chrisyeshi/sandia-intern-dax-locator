#include <vector>
#include <cassert>

#include "../RandomPoints3D.h"
#include "stdnormal_test.h"

int main(void)
{
    int count = 10000;

    // generate the points;
    RandomPoints3D random;
    random.setPointCount(count);
    random.generate();
    std::vector<Point3D> pts = random.getPoints();
    assert(pts.size() == count);

    // extract the 2d points
    std::vector<Point2D> pts2d(count);
    for (unsigned int i = 0; i < count; ++i)
    {
        Point3D pt3d = pts[i];
        Point2D pt2d(pt3d[0], pt3d[1]);
        pts2d[i] = pt2d;
    }

    // test
    float exp_mean[2] = {0.f, 0.f};
    float exp_std[2] = {1.f, 1.f};
    testStdNormal(pts2d, exp_mean, exp_std);

    return 0;
}
