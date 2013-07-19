#include "ExecLocator.h"

#include "BinPoints.h"

///////////////////////////////////////////////////////////////////////
//
//
//
//
// Public Methods
//
//
//
//
//////////////////////////////////////////////////////////////////////

ExecLocator::ExecLocator()
{
}

ExecLocator::ExecLocator(const dax::Vector2& origin,
                         const dax::Vector2& spacing,
                         const dax::Extent3& extent,
                         const std::vector<dax::Vector2>& sortPoints,
                         const std::vector<dax::Id>& pointStarts,
                         const std::vector<int>& pointCounts)
                       : origin(origin),
                         spacing(spacing),
                         extent(extent),
                         sortPoints(sortPoints),
                         pointStarts(pointStarts),
                         pointCounts(pointCounts)
{
}

ExecLocator::~ExecLocator()
{
}

void ExecLocator::setOrigin(const dax::Vector2& origin)
{
    this->origin = origin;
}

void ExecLocator::setSpacing(const dax::Vector2& spacing)
{
    this->spacing = spacing;
}

void ExecLocator::setExtent(const dax::Extent3& extent)
{
    this->extent = extent;
}

void ExecLocator::setSortPoints(const std::vector<dax::Vector2>& sortPoints)
{
    this->sortPoints = sortPoints;
}

void ExecLocator::setPointStarts(const std::vector<dax::Id>& pointStarts)
{
    this->pointStarts = pointStarts;
}

void ExecLocator::setPointCounts(const std::vector<int>& pointCounts)
{
    this->pointCounts = pointCounts;
}

dax::Id ExecLocator::getBucketId(const dax::Vector2& point) const
{
    // make sure the point is within extent
    if (point[0] < extent.Min[0] || point[0] >= extent.Max[0]
     || point[1] < extent.Min[1] || point[1] >= extent.Max[1])
        return -1;
    // use the helper function from Binpoints.h to find the bucket id
    return binPoint(point, origin, spacing, extent);
}

std::vector<dax::Vector2> ExecLocator::getBucketPoints(const dax::Id& bucketId) const
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

// find coincident point
dax::Id ExecLocator::findPoint(const dax::Vector2& point) const
{
    // in which bucket
    dax::Id bucketId = this->getBucketId(point);
    dax::Id start = pointStarts[bucketId];
    int count = pointCounts[bucketId];
    // iterate to find the coincident point
    for (unsigned int i = 0; i < count; ++i)
    {
        dax::Id pointId = i + start;
        dax::Vector2 samplePoint = sortPoints[pointId];
        if (coincident(point, samplePoint))
            return pointId;
    }
    // no coincident point found
    return -1;
}

dax::Vector2 ExecLocator::getPoint(const dax::Id& pointId) const
{
    // check pointId range
    if (pointId < 0 || pointId >= sortPoints.size())
        return dax::Vector2();
    // index sortPoints to get the point
    return sortPoints[pointId];
}

dax::Id ExecLocator::findNearestPoint(const dax::Vector2& point) const
{
    // TODO: not supporting yet
    return -1;
}

///////////////////////////////////////////////////////////////////////
//
//
//
//
// Protected Methods
//
//
//
//
//////////////////////////////////////////////////////////////////////

bool ExecLocator::coincident(const dax::Vector2& point1,
                             const dax::Vector2& point2) const
{
    // two points are coincident when both coordinates are
    // within the tolerence value
    if (fabs(point1[0] - point2[0]) > 0.0001)
        return false;
    if (fabs(point1[1] - point2[1]) > 0.0001)
        return false;
    return true;
}

///////////////////////////////////////////////////////////////////////
//
//
//
//
// Private Methods
//
//
//
//
//////////////////////////////////////////////////////////////////////
