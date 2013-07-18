#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>

#include <dax/Types.h>

#include "Point2D.h"
#include "tests/RandomPoints2D.h"
#include "DaxLocator.h"

// main
int main(void)
{
    // first generate a bunch of random points
    RandomPoints2D random;
    random.setExtent(0, 3, 0, 3);
    random.setPointCount(20);
    random.generate();
    std::vector<Point2D> points = random.getPoints();
    points.push_back(Point2D(0.99, 0.99));

    // translate Point2D to dax::vector2
    std::vector<dax::Vector2> daxPoints(points.size());
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        Point2D point = points[i];
        dax::Vector2 daxvec(point.x(), point.y());
        daxPoints[i] = daxvec;
    }

    // use DaxLocator class
    DaxLocator locator;
    locator.setSpacing(1.0, 1.0);
    locator.setExtent(0, 3, 0, 3);
    locator.setPoints(daxPoints);
    locator.build();

    // outputs
    std::vector<dax::Id> oriBucketIds = locator.getOriBucketIds();
    std::vector<dax::Id> bucketIds = locator.getBucketIds();
    std::vector<dax::Vector2> sortPoints = locator.getSortPoints();
    std::vector<dax::Id> uniqueBucketIds = locator.getUniqueBucketIds();
    std::vector<dax::Id> pointStartIds = locator.getPointStartIds();
    std::vector<int> bucketPointCounts = locator.getBucketPointCounts();

    // print
    std::cout.precision(4);
    std::cout << std::setw(10) << "Buckets: ";
    for (unsigned int i = 0; i < oriBucketIds.size(); ++i)
        std::cout << std::setw(6) << oriBucketIds[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Ori X: ";
    for (unsigned int i = 0; i < daxPoints.size(); ++i)
        std::cout << std::setw(6) << daxPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Ori Y: ";
    for (unsigned int i = 0; i < daxPoints.size(); ++i)
        std::cout << std::setw(6) << daxPoints[i][1] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Buckets: ";
    for (unsigned int i = 0; i < bucketIds.size(); ++i)
        std::cout << std::setw(6) << bucketIds[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Sort X: ";
    for (unsigned int i = 0; i < sortPoints.size(); ++i)
        std::cout << std::setw(6) << sortPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Sort Y: ";
    for (unsigned int i = 0; i < sortPoints.size(); ++i)
        std::cout << std::setw(6) << sortPoints[i][1] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Unique: ";
    for (unsigned int i = 0; i < uniqueBucketIds.size(); ++i)
        std::cout << std::setw(3) << uniqueBucketIds[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Start: ";
    for (unsigned int i = 0; i < pointStartIds.size(); ++i)
        std::cout << std::setw(3) << pointStartIds[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Count: ";
    for (unsigned int i = 0; i < bucketPointCounts.size(); ++i)
        std::cout << std::setw(3) << bucketPointCounts[i] << ", ";
    std::cout << std::endl;

    // binning a point 
    while (true)
    {
        // inputs for binPoint
        float x, y;
        std::cout << std::setw(10) << "X: ";
        std::cin >> x;
        std::cout << std::setw(10) << "Y: ";
        std::cin >> y;
        dax::Vector2 point(x, y);
        // find the bucket id the point belongs to
        dax::Id id = locator.locatePoint(point);
        // find the points in the same bucket
        std::vector<dax::Vector2> points = locator.getBucketPoints(id);
        
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
    }

    return 0;
}
