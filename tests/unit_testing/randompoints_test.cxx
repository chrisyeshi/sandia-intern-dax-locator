#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <cmath>

#include "../RandomPoints2D.h"
#include "stdnormal_test.h"

#define equal_point(pt1, pt2) assert(fabs(pt1.x() - pt2.x()) < 0.0001 && fabs(pt1.y() - pt2.y()) < 0.0001);

int main(void)
{
    int count = 10000;

    // generate the points
    RandomPoints2D random;
    random.setExtent(0, 99, 0, 99);
    random.setPointCount(count);
    random.generate();
    std::vector<Point2D> pts1 = random.getPoints();

    // test
    float exp_mean[2] = {50.f, 50.f};
    float exp_std[2] = {16.66, 16.66};
    testStdNormal(pts1, exp_mean, exp_std);

    // test read and write
    // first write to a file
    random.write("randompoints.dat");
    // then read from the file using the same variable
    random.read("randompoints.dat");
    std::vector<Point2D> pts2 = random.getPoints();
    // read from the file using a new variable
    RandomPoints2D reader;
    reader.read("randomponits.dat");
    std::vector<Point2D> pts3 = random.getPoints();
    // they should be all equal
    assert(pts1.size() == count);
    assert(pts2.size() == count);
    assert(pts3.size() == count);
    for (unsigned int i = 0; i < count; ++i)
    {
        equal_point(pts1[i], pts2[i]);
        equal_point(pts2[i], pts3[i]);
        equal_point(pts1[i], pts3[i]);
    }
    
    return 0;
}
