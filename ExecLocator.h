#ifndef __EXECLOCATOR_H__
#define __EXECLOCATOR_H__

#include <vector>

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/cont/arg/ExecutionObject.h>

class ExecLocator : public dax::exec::ExecutionObjectBase
{
public:
    ExecLocator();
    ExecLocator(const dax::Vector2& origin,
                const dax::Vector2& spacing,
                const dax::Extent3& extent,
                const std::vector<dax::Vector2>& sortPoints,
                const std::vector<dax::Id>& pointStarts,
                const std::vector<int>& pointCounts);
    virtual ~ExecLocator();

    void setOrigin(const dax::Vector2& origin);
    void setSpacing(const dax::Vector2& spacing);
    void setExtent(const dax::Extent3& extent);
    void setSortPoints(const std::vector<dax::Vector2>& sortPoints);
    void setPointStarts(const std::vector<dax::Id>& pointStarts);
    void setPointCounts(const std::vector<int>& pointCounts);

    dax::Id getBucketId(const dax::Vector2& point) const;
    std::vector<dax::Vector2> getBucketPoints(const dax::Id& bucketId) const;
    dax::Id findPoint(const dax::Vector2& point) const;
    dax::Vector2 getPoint(const dax::Id& pointId) const;
    dax::Id findNearestPoint(const dax::Vector2& point) const;

protected:
    dax::Vector2 origin;
    dax::Vector2 spacing;
    dax::Extent3 extent;
    std::vector<dax::Vector2> sortPoints;
    std::vector<dax::Id> pointStarts;
    std::vector<int> pointCounts;

    bool coincident(const dax::Vector2& point1, const dax::Vector2& point2) const;

private:
};

#endif //__EXECLOCATOR_H__
