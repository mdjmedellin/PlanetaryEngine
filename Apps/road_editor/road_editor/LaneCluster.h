#pragma once
#ifndef LANE_CLUSTER_H
#define LANE_CLUSTER_H

//includes
#include <vector>
//

class Vector3;

namespace gh
{
	//=================================FORWARD DECLARATION=============================
	class DrivingLane;
	class MatrixStack;
	class RoadSpline;
	class Road;
	//=================================================================================

	class LaneCluster
	{
	public:
		LaneCluster();
		void update( double deltaTime );
		void render( MatrixStack& currentMatrixStack, float scale );
		DrivingLane* getRandomDrivingLane();
		void pushDrivingLane( DrivingLane* drivingLaneAdded );
		void setParentRoad( Road* parentRoad );
		void getAllDrivingLanes( std::vector< DrivingLane* >& out_drivingLaneContainer );

	private:
		void initializeLanes();

		std::vector< DrivingLane* > m_lanes;
	};
}

#endif