#pragma once
#ifndef ROAD_H
#define ROAD_H

//includes
#include <vector>
//

class Vector3;

namespace gh
{
	//======================================FORWARD DECLARATION============================
	class MatrixStack;
	class Vehicle;
	class DrivingLane;
	class LaneCluster;
	//=====================================================================================

	class Road
	{
	public:
		Road();
		void update( double deltaTime );
		void render( MatrixStack& currentMatrixStack, float scale );
		bool exitRoad( Vehicle* exitingVehicle );
		void placeVehicleRandomly( Vehicle* vehicleToPlace );
		void pushLaneCluster( LaneCluster* laneClusterAdded );
		void getAllDrivingLanes( std::vector< DrivingLane* >& out_drivingLanesContainer );
		DrivingLane* getRandomDrivingLane();

	private:
		void initiateLaneClusters();

		std::vector< Vehicle* > m_vehiclesOnRoad;
		std::vector< LaneCluster* > m_laneClusters;
	};
}

#endif