#pragma once
#ifndef VEHICLE_MANAGER_H
#define VEHICLE_MANAGER_H

//////////////////////////////////////////////////////////////////////////
//==============================INCLUDES==================================
#include <vector>
//////////////////////////////////////////////////////////////////////////

namespace gh
{
	//////////////////////////////////////////////////////////////////////////
	//========================FORWARD DECLARATIONS============================
	class Vehicle;
	class RoadSystem;
	class MatrixStack;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//===========================VEHICLE MANAGER==============================
	class VehicleManager
	{
	public:
		VehicleManager( RoadSystem* roadSystem, size_t desiredNumberOfVehicles = 8 );
		~VehicleManager();

		void updateVehicles( double deltaTime );
		void renderVehicles( MatrixStack& currentMatrixStack, float scale );

	private:
		int removeVehiclesThatReachedTheirDestination();
		int spawnMaxNumberOfVehicles();

		RoadSystem* m_roadSystem;
		std::vector< Vehicle* > m_vehicles;
		size_t m_desiredVehicles;
	};
	//////////////////////////////////////////////////////////////////////////
}

#endif