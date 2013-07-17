#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

#include "../../Point2D.h"

void testStdNormal(const std::vector<Point2D>& pts,
                   float exp_mean[2], float exp_std[2])
{
    // test if they are normally distributed
    // using the mean and variance test
    // calculate the sum
    float sum[2];
    sum[0] = sum[1] = 0.f;
    for (unsigned int i = 0; i < pts.size(); ++i)
    {
        sum[0] += pts[i].x();
        sum[1] += pts[i].y();
    }
    // calculate and test the mean
    float mean[2];
    for (int i = 0; i < 2; ++i)
    {
        mean[i] = sum[i] / float(pts.size());
        std::cout << "Mean[" << i << "] = " << mean[i] << std::endl;
        assert(fabs(mean[i] - exp_mean[i]) < 0.3);
    }
    // calculate and test the variance
    // using sum(X*x)/N-mean*mean
    float squaresum[2];
    squaresum[0] = squaresum[1] = 0.f;
    for (unsigned int i = 0; i < pts.size(); ++i)
    {
        squaresum[0] += pts[i].x() * pts[i].x();
        squaresum[1] += pts[i].y() * pts[i].y();
    }
    float standard[2];
    for (int i = 0; i < 2; ++i)
    {
        standard[i] = sqrt(squaresum[i] / float(pts.size()) - mean[i] * mean[i]);
        std::cout << "Std[" << i << "] = " << standard[i] << std::endl;
        assert(fabs(standard[i] - exp_std[i]) < 0.1);
    }
}
