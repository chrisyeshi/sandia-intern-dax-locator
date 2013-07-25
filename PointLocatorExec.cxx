#define BOOST_SP_DISABLE_THREADS

#include "PointLocatorExec.h"

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

PointLocatorExec::PointLocatorExec()
{
}

PointLocatorExec::PointLocatorExec(const dax::Vector3& origin,
                         const dax::Vector3& spacing,
                         const dax::Extent3& extent,
                         ArrayHandle<dax::Vector3>::PortalConstExecution sortPoints,
                         ArrayHandle<dax::Id>::PortalConstExecution pointStarts,
                         ArrayHandle<int>::PortalConstExecution pointCounts)
                       : origin(origin),
                         spacing(spacing),
                         extent(extent),
                         sortPoints(sortPoints),
                         pointStarts(pointStarts),
                         pointCounts(pointCounts)
{
}

void PointLocatorExec::setOrigin(const dax::Vector3& origin)
{
    this->origin = origin;
}

void PointLocatorExec::setSpacing(const dax::Vector3& spacing)
{
    this->spacing = spacing;
}

void PointLocatorExec::setExtent(const dax::Extent3& extent)
{
    this->extent = extent;
}

void PointLocatorExec::setSortPoints
    (ArrayHandle<dax::Vector3>::PortalConstExecution sortPoints)
{
    this->sortPoints = sortPoints;
}

void PointLocatorExec::setPointStarts
    (ArrayHandle<dax::Id>::PortalConstExecution pointStarts)
{
    this->pointStarts = pointStarts;
}

void PointLocatorExec::setPointCounts
    (ArrayHandle<int>::PortalConstExecution pointCounts)
{
    this->pointCounts = pointCounts;
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
