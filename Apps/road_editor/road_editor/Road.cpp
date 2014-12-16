#include "stdafx.h"
#include "Road.h"
#include "LaneCluster.h"
#include "Vector3.hpp"
#include "MathUtilities.hpp"
#include "LaneNode.h"
#include "Vehicle.h"
#include "DrivingLane.h"
#include "RoadSpline.h"

namespace gh
{
	Road::Road()
		:	m_laneClusters( std::vector< LaneCluster* >() )
		,	m_vehiclesOnRoad( std::vector< Vehicle* >() )
	{}

	void Road::render( MatrixStack& currentMatrixStack, float scale )
	{
		for( std::vector< LaneCluster* >::iterator currentLaneCluster = m_laneClusters.begin(); currentLaneCluster != m_laneClusters.end(); ++currentLaneCluster )
		{
			(*currentLaneCluster)->render( currentMatrixStack, scale );
		}
	}

	bool Road::exitRoad( Vehicle* exitingVehicle )
	{
		bool erasedVehicle = false;

		for( std::vector< Vehicle* >::iterator currentVehicle = m_vehiclesOnRoad.begin();
			currentVehicle != m_vehiclesOnRoad.end(); )
		{
			if( ( *currentVehicle ) == exitingVehicle )
			{
				currentVehicle = m_vehiclesOnRoad.erase( currentVehicle );
				return true;
			}
			else
			{
				++currentVehicle;
			}
		}

		return false;
	}

	DrivingLane* Road::getRandomDrivingLane()
	{
		//select a random cluster
		if( !m_laneClusters.empty() )
		{
			int laneCluster = int( RoundFloatToNearestInt( RandZeroToOne() * float( m_laneClusters.size() - 1 ) ) );
			return m_laneClusters[ laneCluster ]->getRandomDrivingLane();
		}
		else
		{
			return nullptr;
		}
	}

	void Road::pushLaneCluster( LaneCluster* laneClusterAdded )
	{
		m_laneClusters.push_back( laneClusterAdded );
		laneClusterAdded->setParentRoad( this );
	}

	void Road::getAllDrivingLanes( std::vector< DrivingLane* >& out_drivingLanesContainer )
	{
		for( std::vector< LaneCluster* >::iterator currentLaneCluster = m_laneClusters.begin();
			currentLaneCluster != m_laneClusters.end(); ++currentLaneCluster )
		{
			(*currentLaneCluster)->getAllDrivingLanes( out_drivingLanesContainer );
		}
	}

	void Road::update( double deltaTime )
	{
		for( std::vector< LaneCluster* >::iterator currentLaneCluster = m_laneClusters.begin();
			currentLaneCluster != m_laneClusters.end(); ++currentLaneCluster )
		{
			(*currentLaneCluster)->update( deltaTime );
		}
	}

}