#pragma once
#ifndef DRIVING_LANE_H
#define DRIVING_LANE_H

//===============================INCLUDES=====================================
#include <vector>
#include <string>
#include "RoadSpline.h"
#include "DrivingSegment.h"
//============================================================================

//=============================FORWARD DECLARATION=============================
class Vector3;
//=============================================================================

namespace gh
{
	//========================================FORWARD DECLARATION=====================================
	class LaneNode;
	class MatrixStack;
	class Intersection;
	class Road;
	class Vehicle;
	//=================================================================================================


	//=======================================CLASS DECLARATION=========================================
	class DrivingLane : public DrivingSegment
	{
	public:
		DrivingLane( int id );
		void pushLaneNode( LaneNode* newLaneNode );
		void getLastTwoLaneNodes( LaneNode*& out_penultimateLaneNode, LaneNode*& out_lastLaneNode );
		void getFirstTwoLaneNodes( LaneNode*& out_firstLaneNode, LaneNode*& out_secondLaneNode );
		void setParentRoad( Road* parentRoad );
		void setConnectingLane( DrivingLane* connectingLane );
		virtual int getID();
		virtual bool placeVehicleRandomly( Vehicle* vehicleToPlace );
		virtual bool placeVehicleAtSpecifiedLength( Vehicle* vehicleToPlace );
		virtual Intersection* getEndIntersection();
		void initializeSpline();

		Intersection* m_intersection;

	private:
		int m_id;
		Road* m_parentRoad;
		std::vector< LaneNode* > m_laneNodes;
		std::vector< DrivingLane* > m_connectingLanes;
	};
	//==================================================================================================
}

#endif