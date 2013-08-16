#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>

#include <dax/Types.h>

#include "Point3D.h"
#include "tests/RandomPoints3D.h"
#include "PointLocator.h"

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

    // translate Point2D to dax::vector2
    std::vector<dax::Vector3> daxPoints(points.size());
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        Point3D point = points[i];
        dax::Vector3 daxvec(point.x(), point.y(), point.z());
        daxPoints[i] = daxvec;
    }
    
    // print input
    std::cout << std::fixed;
    std::cout.precision(4);
    std::cout << std::setw(10) << "X: ";
    for (unsigned int i = 0; i < daxPoints.size(); ++i)
        std::cout << std::setw(6) << daxPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Y: ";
    for (unsigned int i = 0; i < daxPoints.size(); ++i)
        std::cout << std::setw(6) << daxPoints[i][1] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Z: ";
    for (unsigned int i = 0; i < daxPoints.size(); ++i)
        std::cout << std::setw(6) << daxPoints[i][2] << ", ";
    std::cout << std::endl;

    // use PointLocator class
    PointLocator locator;
    locator.setDimensions(3, 3, 3);
    locator.setBounds(3, 3, 3);
    locator.setPoints(daxPoints);
    locator.build();

    // outputs
    std::vector<dax::Vector3> sortPoints = locator.getSortPoints();
    std::vector<dax::Id> pointStarts = locator.getPointStarts();
    std::vector<int> pointCounts = locator.getPointCounts();

    // print
    std::cout << std::fixed;
    std::cout.precision(4);
    std::cout << std::setw(10) << "Sort X: ";
    for (unsigned int i = 0; i < sortPoints.size(); ++i)
        std::cout << std::setw(6) << sortPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Sort Y: ";
    for (unsigned int i = 0; i < sortPoints.size(); ++i)
        std::cout << std::setw(6) << sortPoints[i][1] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Sort Z: ";
    for (unsigned int i = 0; i < sortPoints.size(); ++i)
        std::cout << std::setw(6) << sortPoints[i][2] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Pt Start: ";
    for (unsigned int i = 0; i < pointStarts.size(); ++i)
        std::cout << std::setw(3) << pointStarts[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Pt Count: ";
    for (unsigned int i = 0; i < pointCounts.size(); ++i)
        std::cout << std::setw(3) << pointCounts[i] << ", ";
    std::cout << std::endl;

    // binning a point 
    while (true)
    {
        // inputs for binPoint
        float x, y, z;
        std::cout << std::setw(10) << "X: ";
        std::cin >> x;
        std::cout << std::setw(10) << "Y: ";
        std::cin >> y;
        std::cout << std::setw(10) << "Z: ";
        std::cin >> z;
        dax::Vector3 point(x, y, z);
        // find the bucket id the point belongs to
        dax::Id id = locator.locatePoint(point);
        // find the points in the same bucket
        std::vector<dax::Vector3> points = locator.getBucketPoints(id);
        
        // print 
        std::cout << std::setw(10) << "Bucket Id: " << id << std::endl;
        std::cout << std::setw(10) << "Pts X: ";
        for (unsigned int i = 0; i < points.size(); ++i)
            std::cout << std::setw(6) << points[i][0] << ", ";
        std::cout << std::endl;
        std::cout << std::setw(10) << "Pts Y: ";
        for (unsigned int i = 0; i < points.size(); ++i)
            std::cout << std::setw(6) << points[i][1] << ", ";
        std::cout << std::endl;
        std::cout << std::setw(10) << "Pts Z: ";
        for (unsigned int i = 0; i < points.size(); ++i)
            std::cout << std::setw(6) << points[i][2] << ", ";
        std::cout << std::endl;
    }

    return 0;
}
