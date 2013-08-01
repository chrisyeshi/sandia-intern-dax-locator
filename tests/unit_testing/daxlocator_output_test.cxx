#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <dax/Types.h>

#include "Point3D.h"
#include "tests/RandomPoints3D.h"
#include "PointLocator.h"
#include "tests/unit_testing/Help.h"

// main
int main(void)
{
    // first generate a bunch of random points
    RandomPoints3D random;
    random.setExtent(0, 3, 0, 3, 0, 3);
    random.setPointCount(20);
    random.generate();
    std::vector<Point3D> points = random.getPoints();
    points.push_back(Point3D(0.99, 0.99, 0.99));

    // translate Point3D to dax::vector3
    std::vector<dax::Vector3> daxPoints(points.size());
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        Point3D point = points[i];
        dax::Vector3 daxvec(point.x(), point.y(), point.z());
        daxPoints[i] = daxvec;
    }

    // use PointLocator class
    PointLocator locator;
    locator.setAutomatic(false);
    locator.setDimensions(3, 3, 3);
    locator.setBounds(3, 3, 3);
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
    help::printBinPoint(1.f, 1.f, 1.f, locator, ss);

    // compare the output and the correct output 
    help::printCompare(ss.str(), "daxlocator_correct_output.txt");

    return 0;
}
