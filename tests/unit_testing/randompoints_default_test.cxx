#include <vector>

#include "../RandomPoints2D.h"
#include "stdnormal_test.h"

int main(void)
{
    int count = 10000;

    // generate the points
    RandomPoints2D random;
    random.setPointCount(count);
    random.generate();
    std::vector<Point2D> pts = random.getPoints();

    // test 
    float exp_mean[2] = {0.f, 0.f};
    float exp_std[2] = {1.f, 1.f};
    testStdNormal(pts, exp_mean, exp_std);

    return 0;
}
