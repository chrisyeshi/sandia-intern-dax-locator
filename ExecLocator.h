#ifndef __EXECLOCATOR_H__
#define __EXECLOCATOR_H__

#include <dax/Types.h>
#include <dax/Extent.h>
#include <dax/cont/arg/ExecutionObject.h>
#include <dax/math/Precision.h>
#include <dax/cont/ArrayHandle.h>

using namespace dax::cont;

class ExecLocator : public dax::exec::ExecutionObjectBase
{
public:
    ExecLocator();
    ExecLocator(const dax::Vector2& origin,
                const dax::Vector2& spacing,
                const dax::Extent3& extent,
                ArrayHandle<dax::Vector2>::PortalConstExecution sortPoints,
                ArrayHandle<dax::Id>::PortalConstExecution pointStarts,
                ArrayHandle<int>::PortalConstExecution pointCounts);

    void setOrigin(const dax::Vector2& origin);
    void setSpacing(const dax::Vector2& spacing);
    void setExtent(const dax::Extent3& extent);
    void setSortPoints(ArrayHandle<dax::Vector2>::PortalConstExecution sortPoints);
    void setPointStarts(ArrayHandle<dax::Id>::PortalConstExecution pointStarts);
    void setPointCounts(ArrayHandle<int>::PortalConstExecution pointCounts);

    // find bucket Id that a point is in
    DAX_EXEC_EXPORT
    dax::Id getBucketId(const dax::Vector2& point) const
    {
        // make sure the point is within extent
        if (point[0] < extent.Min[0] || point[0] >= extent.Max[0]
         || point[1] < extent.Min[1] || point[1] >= extent.Max[1])
            return -1;
        // use the helper function from Binpoints.h to find the bucket id
        return binPoint(point);
    }

    // get bucket point count by bucket id
    DAX_EXEC_EXPORT
    int getBucketPointCount(const dax::Id& bucketId) const
    {
        return pointCounts.Get(bucketId);
    }

    // given a bucket id, we can get the points in this bucket
    // TODO: disabled to make sure CUDA works first,
    // come back later to make arrays work
/*
    DAX_EXEC_EXPORT
    std::vector<dax::Vector2> getBucketPoints(const dax::Id& bucketId) const
    {
        // get the starting id and point count
        dax::Id start = pointStarts[bucketId];
        int count = pointCounts[bucketId];
        // if there is no point, return an empty array
        if (0 == count)
            return std::vector<dax::Vector2>();
        // iterate to get the points 
        std::vector<dax::Vector2> retPoints(count);
        for (unsigned int i = 0; i < count; ++i)
            retPoints[i] = sortPoints[i + start];
        return retPoints;
    }
*/
    
    // find coincident point
    DAX_EXEC_EXPORT dax::Id findPoint(const dax::Vector2& point) const
    {
        // in which bucket
        dax::Id bucketId = this->getBucketId(point);
        dax::Id start = pointStarts.Get(bucketId);
        int count = pointCounts.Get(bucketId);
        // iterate to find the coincident point
        for (unsigned int i = 0; i < count; ++i)
        {
            dax::Id pointId = i + start;
            dax::Vector2 samplePoint = sortPoints.Get(pointId);
            if (coincident(point, samplePoint))
                return pointId;
        }
        // no coincident point found
        return -1;
    }

    // get a point by the point Id of sortPoints
    DAX_EXEC_EXPORT dax::Vector2 getPoint(const dax::Id& pointId) const
    {
        // check pointId range
        if (pointId < 0 || pointId >= sortPoints.GetNumberOfValues())
            return dax::Vector2();
        // index sortPoints to get the point
        return sortPoints.Get(pointId);
    }

    // TODO: not supporting yet...
    DAX_EXEC_EXPORT dax::Id findNearestPoint(const dax::Vector2& point) const
    {
        return -1;
    }

protected:
    dax::Vector2 origin;
    dax::Vector2 spacing;
    dax::Extent3 extent;
    ArrayHandle<dax::Vector2>::PortalConstExecution sortPoints;
    ArrayHandle<dax::Id>::PortalConstExecution pointStarts;
    ArrayHandle<int>::PortalConstExecution pointCounts;

    DAX_EXEC_CONT_EXPORT
    bool coincident(const dax::Vector2& point1, const dax::Vector2& point2) const
    {
        // two points are coincident when both coordinates are
        // within the tolerence value
        if (fabs(point1[0] - point2[0]) > 0.0001)
            return false;
        if (fabs(point1[1] - point2[1]) > 0.0001)
            return false;
        return true;
    }

    DAX_EXEC_CONT_EXPORT
    dax::Id binPoint(const dax::Vector2& point) const
    {
        int resolution[2] = {extent.Max[0] - extent.Min[0],
                             extent.Max[1] - extent.Min[0]};
        // compute the point coordinate within the grid
        dax::Vector2 coord(point[0] - origin[0], point[1] - origin[1]);
        // which cell the point belongs
        dax::Id xid, yid;
        xid = dax::math::Floor(coord[0] / spacing[0]);
        yid = dax::math::Floor(coord[1] / spacing[1]);
        return xid + yid * resolution[0];
    }

private:
};

#endif //__EXECLOCATOR_H__
