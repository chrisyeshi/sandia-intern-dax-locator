#define BOOST_SP_DISABLE_THREADS

#include "ExecLocator.h"

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

ExecLocator::ExecLocator(const dax::Vector3& origin,
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

void ExecLocator::setOrigin(const dax::Vector3& origin)
{
    this->origin = origin;
}

void ExecLocator::setSpacing(const dax::Vector3& spacing)
{
    this->spacing = spacing;
}

void ExecLocator::setExtent(const dax::Extent3& extent)
{
    this->extent = extent;
}

void ExecLocator::setSortPoints(ArrayHandle<dax::Vector3>::PortalConstExecution sortPoints)
{
    this->sortPoints = sortPoints;
}

void ExecLocator::setPointStarts(ArrayHandle<dax::Id>::PortalConstExecution pointStarts)
{
    this->pointStarts = pointStarts;
}

void ExecLocator::setPointCounts(ArrayHandle<int>::PortalConstExecution pointCounts)
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
