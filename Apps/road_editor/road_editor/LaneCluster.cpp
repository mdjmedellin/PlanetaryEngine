#include "stdafx.h"
#include "LaneCluster.h"
#include "DrivingLane.h"
#include "Vector3.hpp"
#include "MathUtilities.hpp"
#include "Road.h"

namespace gh
{
	LaneCluster::LaneCluster()
		:	m_lanes( std::vector< DrivingLane* >() )
	{
		//initializeLanes();
	}

	void LaneCluster::initializeLanes()
	{
		//this is for debugging purposes
		//m_lanes.push_back( new DrivingLane() );
	}

	void LaneCluster::render( MatrixStack& currentMatrixStack, float scale )
	{
		for( std::vector< DrivingLane* >::iterator currentLane = m_lanes.begin(); currentLane != m_lanes.end(); ++currentLane )
		{
			(*currentLane)->render( currentMatrixStack, scale );
		}
	}

	DrivingLane* LaneCluster::getRandomDrivingLane()
	{
		if( !m_lanes.empty() )
		{
			int lane = int( RoundFloatToNearestInt( RandZeroToOne() * float( m_lanes.size() - 1 ) ) );
			return m_lanes[ lane ];
		}
		else
		{
			return nullptr;
		}
	}

	void LaneCluster::pushDrivingLane( DrivingLane* drivingLaneAdded )
	{
		m_lanes.push_back( drivingLaneAdded );
		m_lanes.back()->initializeSpline();
	}

	void LaneCluster::setParentRoad( Road* parentRoad )
	{
		for( std::vector< DrivingLane* >::iterator currentDrivingLane = m_lanes.begin();
			currentDrivingLane != m_lanes.end(); ++currentDrivingLane )
		{
			(*currentDrivingLane)->setParentRoad( parentRoad );
		}
	}

	void LaneCluster::getAllDrivingLanes( std::vector< DrivingLane* >& out_drivingLaneContainer )
	{
		for( std::vector< DrivingLane* >::iterator currentLane = m_lanes.begin(); currentLane != m_lanes.end(); ++currentLane )
		{
			out_drivingLaneContainer.push_back( (*currentLane) );
		}
	}

	void LaneCluster::update( double deltaTime )
	{
		for( std::vector< DrivingLane* >::iterator currentLane = m_lanes.begin(); currentLane != m_lanes.end(); ++currentLane )
		{
			(*currentLane )->update( deltaTime );
		}
	}

}