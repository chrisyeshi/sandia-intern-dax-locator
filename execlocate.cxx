#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>

#include <dax/Types.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/arg/ExecutionObject.h>
#include <dax/exec/WorkletMapField.h>
#include <dax/cont/Timer.h>

#include "Point3D.h"
#include "tests/RandomPoints3D.h"
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
    void operator()(const dax::Vector3& point,
                    const Functor& execLocator,
                    dax::Id& bucketId,
                    int& pointCount,
                    dax::Vector3& coinPoint) const
    {
        bucketId = execLocator.getBucketId(point);
        pointCount = execLocator.getBucketPointCount(bucketId);
        dax::Id coinId = execLocator.findPoint(point);
        if (coinId < 0)
            coinPoint = dax::make_Vector3(-1.f, -1.f, -1.f);
        else
            coinPoint = execLocator.getPoint(coinId);
    } 
};

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

    // initialize the dax::cont::Timer
    Timer<> timer;
    timer.Reset();

    // use DaxLocator class
    DaxLocator locator;
    locator.setSpacing(1.0, 1.0, 1.0);
    locator.setExtent(0, 3, 0, 3, 0, 3);
    locator.setPoints(daxPoints);
    locator.build();

    std::cout.precision(4);
    std::cout << std::fixed;
    // search strcuture construction timing result
    dax::Scalar time_construct = timer.GetElapsedTime();
    std::cout << std::setw(10) << "Const: " << std::setw(6) << time_construct << std::endl;

    // outputs
    std::vector<dax::Vector3> sortPoints = locator.getSortPoints();
    std::vector<dax::Id> pointStarts = locator.getPointStarts();
    std::vector<int> pointCounts = locator.getPointCounts();

    // print
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

    // setup the ExecLocator, which is a ExecutionObject
    ExecLocator execLocator = locator.prepareExecutionObject();

    // use the local TestWorklet to test the ExecLocator
    // 1. create test inputs, which are points
    std::vector<dax::Vector3> testPoints(19);
    for (int i = 0; i < 19; ++i)
        testPoints[i] = daxPoints[i];
    testPoints.push_back(dax::make_Vector3(0.0, 0.0, 0.0));
    ArrayHandle<dax::Vector3> hTestPoints = make_ArrayHandle(testPoints);
    // 2. create output array handles
    ArrayHandle<dax::Id> hTestBucketIds;
    ArrayHandle<int> hTestCounts;
    ArrayHandle<dax::Vector3> hTestCoinPoints;
    // 3. run the worklet
    Scheduler<> scheduler;
    scheduler.Invoke(TestWorklet(), hTestPoints, execLocator,
                     hTestBucketIds, hTestCounts, hTestCoinPoints);
    // 4. copy the output
    std::vector<dax::Id> testBucketIds(hTestBucketIds.GetNumberOfValues());
    std::vector<int> testCounts(hTestCounts.GetNumberOfValues());
    std::vector<dax::Vector3> testCoinPoints(hTestCoinPoints.GetNumberOfValues());
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
    std::cout << std::setw(10) << "Coin Z: ";
    for (unsigned int i = 0; i < testCoinPoints.size(); ++i)
        std::cout << std::setw(6) << testCoinPoints[i][2] << ", ";
    std::cout << std::endl;

    return 0;
}
