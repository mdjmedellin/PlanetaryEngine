#include "stdafx.h"
#include "DrivingLane.h"
#include "MatrixStack.h"
#include "LaneNode.h"
#include <vector>
#include "Vector3.hpp"
#include "MathUtilities.hpp"
#include "Renderer.h"
#include "Rgba.h"
#include "RoadSpline.h"
#include "vehicle.h"

namespace gh
{
	DrivingLane::DrivingLane( int id )
		:	DrivingSegment()
		,	m_laneNodes( std::vector< LaneNode* >() )
		,	m_id( id )
		,	m_intersection( nullptr )
	{}

	void DrivingLane::pushLaneNode( LaneNode* laneNode )
	{
		m_laneNodes.push_back( laneNode );
		m_roadSpline->pushLaneNode( laneNode );
	}

	void DrivingLane::getLastTwoLaneNodes( LaneNode*& out_penultimateLaneNode, LaneNode*& out_lastLaneNode )
	{
		assert( m_laneNodes.size() >= 4 );

		size_t maxIndex = m_laneNodes.size() - 1;

		out_lastLaneNode = m_laneNodes[ maxIndex ];
		out_penultimateLaneNode = m_laneNodes[ maxIndex - 1 ];
	}

	void DrivingLane::getFirstTwoLaneNodes( LaneNode*& out_firstLaneNode, LaneNode*& out_secondLaneNode )
	{
		assert( m_laneNodes.size() >= 4 );

		out_firstLaneNode = m_laneNodes[ 0 ];
		out_secondLaneNode = m_laneNodes[ 1 ];
	}

	void DrivingLane::initializeSpline()
	{
		m_roadSpline->createSpline();
	}

	void DrivingLane::setParentRoad( Road* parentRoad )
	{
		m_parentRoad = parentRoad;
	}

	void DrivingLane::setConnectingLane( DrivingLane* connectingLane )
	{
		m_connectingLanes.push_back( connectingLane );
	}

	//debug function
	int DrivingLane::getID()
	{
		return m_id;
	}

	bool DrivingLane::placeVehicleRandomly( Vehicle* vehicleToPlace )
	{
		vehicleToPlace->m_roadDrivingOn = m_parentRoad;
		vehicleToPlace->m_nextIntersection = m_intersection;

		return DrivingSegment::placeVehicleRandomly( vehicleToPlace );
	}

	bool DrivingLane::placeVehicleAtSpecifiedLength( Vehicle* vehicleToPlace )
	{
		vehicleToPlace->m_roadDrivingOn = m_parentRoad;
		vehicleToPlace->m_nextIntersection = m_intersection;

		return DrivingSegment::placeVehicleAtSpecifiedLength(vehicleToPlace);
	}

	Intersection* DrivingLane::getEndIntersection()
	{
		return m_intersection;
	}

}