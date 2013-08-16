#define BOOST_SP_DISABLE_THREADS

#include <iostream>
#include <iomanip>
#include <cassert>

#include <dax/Types.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/arg/ExecutionObject.h>
#include <dax/exec/WorkletMapField.h>

#include "Point3D.h"
#include "tests/RandomPoints3D.h"
#include "PointLocator.h"

using namespace dax::cont;

// only runs in serial, so no cuda test
struct VerifyGetBucketIdExtent : dax::exec::WorkletMapField
{
    // signatures
    typedef void ControlSignature(Field(In), ExecObject(), Field(In));
    typedef void ExecutionSignature(_1, _2, _3);

    // overload operator()
    template<typename ExecType>
    DAX_EXEC_EXPORT
    void operator()(const dax::Vector3& point,
                    const ExecType& locator,
                    const dax::Id& correctId) const
    {
        std::cout << std::setw(15) << "Point: " << point[0] << ", "
                  << point[1] << ", " << point[2] << std::endl;
        dax::Id id = locator.getBucketId(point);
        std::cout << std::setw(15) << "Id: " << id << std::endl;
        std::cout << std::setw(15) << "Correct Id: " << correctId << std::endl;
        assert(id == correctId);
    }
};

// main
int main(void)
{
    // first generate a bunch of random points
    RandomPoints3D random;
    random.setExtent(0, 6, 0, 6, 0, 6);
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
    locator.setBounds(6, 6, 6);
    locator.setPoints(daxPoints);
    locator.build();

    // outputs
    std::vector<dax::Vector3> sortPoints = locator.getSortPoints();
    std::vector<dax::Id> pointStarts = locator.getPointStarts();
    std::vector<int> pointCounts = locator.getPointCounts();

    // setup the PointLocatorExec, which is a ExecutionObject
    PointLocatorExec execLocator = locator.prepareExecutionObject();

    // use VerifyGetBucketIdExtent
    // 1. create test inputs, which are points
    std::vector<dax::Vector3> testPoints;
    testPoints.push_back(dax::make_Vector3(0.0, 0.0, 0.0));
    testPoints.push_back(dax::make_Vector3(2.0, 2.0, 2.0));
    testPoints.push_back(dax::make_Vector3(5.0, 5.0, 5.0));
    testPoints.push_back(dax::make_Vector3(6.1, 6.1, 6.1));
    ArrayHandle<dax::Vector3> hTestPoints = make_ArrayHandle(testPoints);
    // 2. create correct bucket id array
    std::vector<dax::Id> testBuckets;
    testBuckets.push_back(0);
    testBuckets.push_back(13);
    testBuckets.push_back(26);
    testBuckets.push_back(-1);
    ArrayHandle<dax::Id> hTestBuckets = make_ArrayHandle(testBuckets);
    // 3. run the worklet
    Scheduler<> scheduler;
    scheduler.Invoke(VerifyGetBucketIdExtent(),
                     hTestPoints, execLocator, hTestBuckets);

    return 0;
}
