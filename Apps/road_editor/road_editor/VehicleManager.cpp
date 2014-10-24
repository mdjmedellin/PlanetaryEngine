#include "stdafx.h"
#include "VehicleManager.h"
#include "RoadSystem.h"
#include "vehicle.h"
#include "DrivingSegment.h"
#include "FloatRange.h"
#include "Rgba.h"
#include "MathUtilities.hpp"

namespace gh
{
	VehicleManager::VehicleManager( RoadSystem* roadSystem, size_t desiredNumberOfVehicles /*= 8 */ )
		:	m_roadSystem( roadSystem )
		,	m_desiredVehicles( desiredNumberOfVehicles )
		,	m_vehicles( std::vector< Vehicle* >() )
	{
		spawnMaxNumberOfVehicles();
	}

	VehicleManager::~VehicleManager()
	{
		for( std::vector< Vehicle* >::iterator currentVehicle = m_vehicles.begin(); currentVehicle != m_vehicles.end();
			++currentVehicle )
		{
			delete ( *currentVehicle );
		}

		m_vehicles.clear();
	}

	void VehicleManager::updateVehicles( double deltaTime )
	{
		for( std::vector< Vehicle* >::iterator currentVehicle = m_vehicles.begin();
			currentVehicle != m_vehicles.end(); ++currentVehicle )
		{
			(*currentVehicle)->updateSimulation( deltaTime );
		}

		//after updating the vehicles, remove the vehicles that have reached their destination
		if( removeVehiclesThatReachedTheirDestination() != 0 )
		{
			spawnMaxNumberOfVehicles();
		}
	}

	int VehicleManager::removeVehiclesThatReachedTheirDestination()
	{
		int numberOfVehiclesRemoved = 0;

		for( std::vector< Vehicle* >::iterator currentVehicle = m_vehicles.begin();
			currentVehicle != m_vehicles.end(); )
		{
			if( (*currentVehicle)->bReachedDestination )
			{
				(*currentVehicle)->m_currentLane->removeVehicle( *currentVehicle );
				(*currentVehicle)->id = -1;
				delete ( *currentVehicle );
				currentVehicle = m_vehicles.erase( currentVehicle );
				++numberOfVehiclesRemoved;
				//++currentVehicle;
			}
			else
			{
				++currentVehicle;
			}
		}

		return numberOfVehiclesRemoved;
	}

	int VehicleManager::spawnMaxNumberOfVehicles()
	{
		int vehiclesSpawned = 0;
		Rgba vehicleColor;
		FloatRange vehicleVelocityRange( "70~120" );

		for( size_t numberOfLiveVehicles = m_vehicles.size(); numberOfLiveVehicles <= m_desiredVehicles;
			++numberOfLiveVehicles )
		{
			m_vehicles.push_back( new Vehicle( vehicleVelocityRange.GetRandomValueInRange(), Rgba( RandZeroToOne(), RandZeroToOne(), RandZeroToOne() ) ) );
			m_vehicles.back()->setRoadSystemPtr( m_roadSystem );
			++vehiclesSpawned;
		}

		return vehiclesSpawned;
	}

	void VehicleManager::renderVehicles( MatrixStack& currentMatrixStack )
	{
		for( std::vector< Vehicle* >::iterator currentVehicle = m_vehicles.begin();
			currentVehicle != m_vehicles.end(); ++currentVehicle )
		{
			(*currentVehicle)->render( currentMatrixStack );
		}
	}

}