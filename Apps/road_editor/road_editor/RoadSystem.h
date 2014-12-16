#pragma once
#ifndef ROAD_SYSTEM_H
#define ROAD_SYSTEM_H

////////////////////////////////////////////////////////////////////////////////////////
//=======================================INCLUDES=======================================
#include <vector>
#include <map>
#include <string>
//======================================================================================
////////////////////////////////////////////////////////////////////////////////////////

class Vector3;

namespace gh
{
	///////////////////////////////////////////////////////////////////////////////////////
	//========================================FORWARD DECLARATIONS=========================
	class MatrixStack;
	class Road;
	class DrivingLane;
	class Intersection;
	class RoadPath;
	class DrivingSegment;
	//======================================================================================
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//=========================================================ROAD SYSTEM==========================================================
	class RoadSystem
	{
	public:
		RoadSystem();
		~RoadSystem();
		RoadPath* calculatePathTo( int endLaneIdentifier, float splineLocation, DrivingSegment* startingDrivingLane );
		void render( MatrixStack& currentMatrixStack );
		DrivingLane* getRandomLane();
		void addRoad( Road* roadAdded );
		void addIntersection( Intersection* intersection );
		void update( double deltaTime );
		DrivingLane* GetLaneWithSpecifiedIndex(int drivingLaneIndex);

	private:
		std::vector< Road* > m_roads;
		std::map< int, DrivingLane* > m_drivingLanes;
		std::vector< Intersection* > m_intersections;
	};
	//===============================================================================================================================
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif