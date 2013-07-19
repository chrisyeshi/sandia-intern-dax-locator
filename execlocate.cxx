#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>

#include <dax/Types.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/arg/ExecutionObject.h>
#include <dax/exec/WorkletMapField.h>

#include "Point2D.h"
#include "tests/RandomPoints2D.h"
#include "DaxLocator.h"
#include "ExecLocator.h"

using namespace dax::cont;

struct TestWorklet : dax::exec::WorkletMapField
{
    // signatures
    typedef void ControlSignature(Field(In), ExecObject(),
                                  Field(Out), Field(Out), Field(Out));
                               // bucketId,   point count, coincident point
    typedef void ExecutionSignature(_1, _2, _3, _4, _5);

    // overload operator()
    template<typename Functor>
    DAX_EXEC_EXPORT
    void operator()(const dax::Vector2& point,
                    const Functor& execLocator,
                    dax::Id& bucketId,
                    int& pointCount,
                    dax::Vector2& coinPoint) const
    {
        bucketId = execLocator.getBucketId(point);
        pointCount = execLocator.getBucketPoints(bucketId).size();
        dax::Id coinId = execLocator.findPoint(point);
        if (coinId < 0)
            coinPoint = dax::make_Vector2(-1.0, -1.0);
        else
            coinPoint = execLocator.getPoint(coinId);
    } 
};

// main
int main(void)
{
    // first generate a bunch of random points
    RandomPoints2D random;
    random.setExtent(0, 2, 0, 2);
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
    std::vector<dax::Vector2> sortPoints = locator.getSortPoints();
    std::vector<dax::Id> pointStarts = locator.getPointStarts();
    std::vector<int> pointCounts = locator.getPointCounts();

    // print
    std::cout.precision(4);
    std::cout << std::setw(10) << "Sort X: ";
    for (unsigned int i = 0; i < sortPoints.size(); ++i)
        std::cout << std::setw(6) << sortPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Sort Y: ";
    for (unsigned int i = 0; i < sortPoints.size(); ++i)
        std::cout << std::setw(6) << sortPoints[i][1] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Pt Start: ";
    for (unsigned int i = 0; i < pointStarts.size(); ++i)
        std::cout << std::setw(3) << pointStarts[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Pt Count: ";
    for (unsigned int i = 0; i < pointCounts.size(); ++i)
        std::cout << std::setw(3) << pointCounts[i] << ", ";
    std::cout << std::endl;

    // setup the ExecLocator, which is a ExecutionObject
    ExecLocator execLocator = locator.prepareExecutionObject();

    // use the local TestWorklet to test the ExecLocator
    // 1. create test inputs, which are points
    std::vector<dax::Vector2> testPoints = daxPoints;
    for (int i = 0; i < daxPoints.size() / 2; ++i)
        testPoints.pop_back();
    testPoints.push_back(dax::make_Vector2(0.0, 0.0));
    ArrayHandle<dax::Vector2> hTestPoints = make_ArrayHandle(testPoints);
    // 2. create output array handles
    ArrayHandle<dax::Id> hTestBucketIds;
    ArrayHandle<int> hTestCounts;
    ArrayHandle<dax::Vector2> hTestCoinPoints;
    // 3. run the worklet
    Scheduler<> scheduler;
    scheduler.Invoke(TestWorklet(), hTestPoints, execLocator,
                     hTestBucketIds, hTestCounts, hTestCoinPoints);
    // 4. copy the output
    std::vector<dax::Id> testBucketIds(hTestBucketIds.GetNumberOfValues());
    std::vector<int> testCounts(hTestCounts.GetNumberOfValues());
    std::vector<dax::Vector2> testCoinPoints(hTestCoinPoints.GetNumberOfValues());
    hTestBucketIds.CopyInto(testBucketIds.begin());
    hTestCounts.CopyInto(testCounts.begin());
    hTestCoinPoints.CopyInto(testCoinPoints.begin());
    // 5. print
    std::cout << std::setw(10) << "Test X: ";
    for (unsigned int i = 0; i < testPoints.size(); ++i)
        std::cout << std::setw(6) << testPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Test Y: ";
    for (unsigned int i = 0; i < testPoints.size(); ++i)
        std::cout << std::setw(6) << testPoints[i][1] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Bucket: ";
    for (unsigned int i = 0; i < testBucketIds.size(); ++i)
        std::cout << std::setw(6) << testBucketIds[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Count: ";
    for (unsigned int i = 0; i < testCounts.size(); ++i)
        std::cout << std::setw(6) << testCounts[i] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Coin X: ";
    for (unsigned int i = 0; i < testCoinPoints.size(); ++i)
        std::cout << std::setw(6) << testCoinPoints[i][0] << ", ";
    std::cout << std::endl;
    std::cout << std::setw(10) << "Coin Y: ";
    for (unsigned int i = 0; i < testCoinPoints.size(); ++i)
        std::cout << std::setw(6) << testCoinPoints[i][1] << ", ";
    std::cout << std::endl;

    return 0;
}
