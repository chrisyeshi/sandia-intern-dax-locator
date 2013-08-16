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

PointLocatorExec::PointLocatorExec(TopologyStructConstExecution topology,
                         ArrayHandle<dax::Vector3>::PortalConstExecution sortPoints,
                         ArrayHandle<dax::Id>::PortalConstExecution pointStarts,
                         ArrayHandle<int>::PortalConstExecution pointCounts)
                       : topology(topology),
                         sortPoints(sortPoints),
                         pointStarts(pointStarts),
                         pointCounts(pointCounts)
{
}

void PointLocatorExec::setTopology(TopologyStructConstExecution topology)
{
    this->topology = topology;
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
