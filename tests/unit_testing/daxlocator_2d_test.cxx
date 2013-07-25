#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <dax/Types.h>

#include "Point2D.h"
#include "tests/RandomPoints2D.h"
#include "DaxLocator.h"
#include "tests/unit_testing/Help.h"

// main
int main(void)
{
    // first generate a bunch of random 2d points
    RandomPoints2D random;
    random.setExtent(0, 3, 0, 3);
    random.setPointCount(20);
    random.generate();
    std::vector<Point2D> points = random.getPoints();
    points.push_back(Point2D(1.99, 1.99));

    // translate Point2D to dax::vector3
    std::vector<dax::Vector3> daxPoints(points.size());
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        Point2D point = points[i];
        dax::Vector3 daxPoint(point.x(), point.y(), 0.0);
        daxPoints[i] = daxPoint;
    }

    // use DaxLocator class
    DaxLocator locator;
    locator.setDivisions(3, 3, 1);
    locator.setExtent(0, 3, 0, 3, 0, 0);
    locator.setPoints(daxPoints);
    locator.build();

    // outputs
    std::vector<dax::Id> pointStarts = locator.getPointStarts();
    std::vector<int> pointCounts = locator.getPointCounts();

    // print
    std::stringstream ss;
    ss.precision(4);
    ss << std::fixed;
    help::printStartCount(pointStarts, pointCounts, ss);

    // binning a point 
    help::printBinPoint(1.f, 1.f, 0.f, locator, ss);
 
    // compare output to the correct output file
    help::printCompare(ss.str(), "daxlocate_2d_correct_output.txt");

    return 0;
}
