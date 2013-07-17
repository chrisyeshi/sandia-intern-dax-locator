#ifndef __STDNORMAL_TEST_H__
#define __STDNORMAL_TEST_H__

#include <vector>

#include "../../Point2D.h"

void testStdNormal(const std::vector<Point2D>& pts,
                   float exp_mean[2], float exp_std[2]);

#endif
