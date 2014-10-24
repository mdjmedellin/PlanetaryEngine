#pragma once
#ifndef ROAD_PATH_H
#define ROAD_PATH_H

//==========================================INCLUDES=====================================================
#include <vector>
#include "Vector3.hpp"
//=======================================================================================================

namespace gh
{
	//===============================================FORWARD DECLARATION================================================
	class DrivingLane;
	class DrivingSegment;
	//==================================================================================================================

	enum PATH_STATUS{ BUILDING_GOOD, BUILDING_BAD, FOUND_COMPLETE_PATH, NO_MORE_OPEN_NODES };

	struct RoadPathNode
	{
		RoadPathNode* m_parentNode;
		DrivingSegment* m_lane;
		float gCost, hCost, fCost;
	};

	class RoadPath
	{
	public:
		RoadPath( const DrivingSegment* in_startLane, const DrivingSegment* in_endLane );
		void addToOpenList( DrivingSegment* in_drivingLane );
		DrivingSegment* getBestOpenNode();
		PATH_STATUS getPathBuildingStatus();
		DrivingSegment* getNextLaneInPath();
		DrivingSegment* peekNextLaneInPath();

	private:
		std::vector< RoadPathNode* > m_pathDirections;
		std::vector< RoadPathNode* > m_closedList;
		std::vector< RoadPathNode* > m_openList;
		Vector3 m_pathStartLocation;
		const DrivingSegment* m_endLane;
		bool m_endLaneIsOnClosedList;
		RoadPathNode* m_currentNode;

		RoadPathNode* findNodeInOpenList( DrivingSegment* laneToFind );
		void constructPath();

		inline float getManhattanDistance( const Vector3& startLoc, const Vector3& endLoc )
		{
			return abs( startLoc.x - endLoc.x ) + abs( startLoc.y - endLoc.y ) + abs( startLoc.z - endLoc.z );
		}
	};
}


#endif