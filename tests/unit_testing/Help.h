#ifndef __HELP_H__
#define __HELP_H__

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <dax/Types.h>
#include <dax/exec/WorkletMapField.h>

class DaxLocator;

namespace help
{

void printStartCount(const std::vector<dax::Id>& pointStarts,
                     const std::vector<int>& pointCounts,
                     std::iostream& stream);

void printBinPoint(float x, float y, float z,
                   const DaxLocator& locator,
                   std::iostream& stream);

void printCompare(const std::string& output, const std::string& filename);

void printCoinPoints(const std::vector<dax::Vector3>& testPoints,
                     const std::vector<dax::Id>& testBucketIds,
                     const std::vector<int>& testCounts,
                     const std::vector<dax::Vector3>& testCoinPoints,
                     std::iostream& stream);

struct FindPointsWorklet : dax::exec::WorkletMapField
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
            coinPoint = dax::make_Vector3(-1.0, -1.0, -1.0);
        else
            coinPoint = execLocator.getPoint(coinId);
    } 
};

}

#endif //__HELP_H__
