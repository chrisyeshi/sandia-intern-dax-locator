#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <dax/Types.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/arg/ExecutionObject.h>
#include <dax/exec/WorkletMapField.h>

#include "Point3D.h"
#include "tests/RandomPoints3D.h"
#include "PointLocator.h"
#include "tests/unit_testing/Help.h"

using namespace dax::cont;

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
    std::vector<dax::Vector3> sortPoints = locator.getSortPoints();
    std::vector<dax::Id> pointStarts = locator.getPointStarts();
    std::vector<int> pointCounts = locator.getPointCounts();

    // print output to a stringstream for comparison purpose
    std::stringstream ss;
    ss.precision(4);
    ss << std::fixed;
    help::printStartCount(pointStarts, pointCounts, ss);

    // setup the PointLocatorExec, which is a ExecutionObject
    PointLocatorExec execLocator = locator.prepareExecutionObject();

    // use the FindPointsWorklet to test the PointLocatorExec
    // 1. create test inputs, which are points
    std::vector<dax::Vector3> testPoints = daxPoints;
    for (int i = 0; i < daxPoints.size() / 2; ++i)
        testPoints.pop_back();
    testPoints.push_back(dax::make_Vector3(0.0, 0.0, 0.0));
    ArrayHandle<dax::Vector3> hTestPoints = make_ArrayHandle(testPoints);
    // 2. create output array handles
    ArrayHandle<dax::Id> hTestBucketIds;
    ArrayHandle<int> hTestCounts;
    ArrayHandle<dax::Vector3> hTestCoinPoints;
    // 3. run the worklet
    Scheduler<> scheduler;
    scheduler.Invoke(help::FindPointsWorklet(), hTestPoints, execLocator,
                     hTestBucketIds, hTestCounts, hTestCoinPoints);
    // 4. copy the output
    std::vector<dax::Id> testBucketIds(hTestBucketIds.GetNumberOfValues());
    std::vector<int> testCounts(hTestCounts.GetNumberOfValues());
    std::vector<dax::Vector3> testCoinPoints(hTestCoinPoints.GetNumberOfValues());
    hTestBucketIds.CopyInto(testBucketIds.begin());
    hTestCounts.CopyInto(testCounts.begin());
    hTestCoinPoints.CopyInto(testCoinPoints.begin());
    // 5. print
    help::printCoinPoints(testPoints, testBucketIds, testCounts, testCoinPoints, ss);

    // compare to the correct output
    help::printCompare(ss.str(), "execlocate_correct_output.txt");

    return 0;
}
