#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <dax/Types.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/arg/ExecutionObject.h>
#include <dax/exec/WorkletMapField.h>

#include "Point2D.h"
#include "tests/RandomPoints2D.h"
#include "DaxLocator.h"
#include "tests/unit_testing/Help.h"

using namespace dax::cont;

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

    // translate Point2D to dax::vector3
    std::vector<dax::Vector3> daxPoints(points.size());
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        Point2D point = points[i];
        dax::Vector3 daxvec(point.x(), point.y(), 0.f);
        daxPoints[i] = daxvec;
    }

    // use DaxLocator class
    DaxLocator locator;
    locator.setSpacing(1.0, 1.0, 0.0);
    locator.setExtent(0, 3, 0, 3, 0, 1);
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

    // setup the ExecLocator, which is a ExecutionObject
    ExecLocator execLocator = locator.prepareExecutionObject();

    // use the FindPointsWorklet to test the ExecLocator
    // 1. create test inputs, which are points
    std::vector<dax::Vector3> testPoints = daxPoints;
    for (int i = 0; i < daxPoints.size() / 2; ++i)
        testPoints.pop_back();
    testPoints.push_back(dax::make_Vector3(1.0, 0.0, 0.0));
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
    help::printCompare(ss.str(), "execlocate_2d_correct_output.txt");

    return 0;
}
