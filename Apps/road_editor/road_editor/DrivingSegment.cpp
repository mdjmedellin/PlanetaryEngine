#include "StdAfx.h"
#include "DrivingSegment.h"
#include "MatrixStack.h"
#include "vehicle.h"
#include "RoadSpline.h"
#include <cassert>


namespace gh
{
	DrivingSegment::DrivingSegment()
		:	m_roadSpline( new RoadSpline() )
		,	m_dirty( true )
	{}

	DrivingSegment::~DrivingSegment()
	{}

	void DrivingSegment::update( double deltaTime )
	{
		//delta time is not used at the moment

		//if the lane is dirty, then reset the pointers to the vehicle in front of every vehicle in this driving segment
		if( m_dirty )
		{
			for( std::vector< Vehicle* >::iterator currentVehicle = m_roadSpline->m_vehiclesOnSpline.begin();
				currentVehicle != m_roadSpline->m_vehiclesOnSpline.end(); ++currentVehicle )
			{
				(*currentVehicle)->resetPointerToNextvehicle();
			}

			m_dirty = false;
		}
	}

	void DrivingSegment::render( MatrixStack& currentMatrixStack, const Rgba& color /* = Rgba::WHITE */ )
	{
		m_roadSpline->render(currentMatrixStack, color );
	}

	bool DrivingSegment::placeVehicleRandomly( Vehicle* vehicleToPlace )
	{
		bool retVal = false;
		vehicleToPlace->m_lengthTraveledAlongSpline = m_roadSpline->getRandomDistanceWithin();

		//check that the vehicle is placed with enough space for the vehicle in front and back to stop
		for( std::vector< Vehicle* >::iterator currentVehicle = m_roadSpline->m_vehiclesOnSpline.begin();
			currentVehicle != m_roadSpline->m_vehiclesOnSpline.end(); ++currentVehicle )
		{
			if( fabs( (*currentVehicle)->m_lengthTraveledAlongSpline - vehicleToPlace->m_lengthTraveledAlongSpline ) < 40.f )
			{
				return false;
			}
		}

		retVal = m_roadSpline->getPosition( vehicleToPlace->m_lengthTraveledAlongSpline,
											vehicleToPlace->m_currentLocation );

		if( retVal )
		{
			m_roadSpline->m_vehiclesOnSpline.push_back( vehicleToPlace );
			m_dirty = true;
		}

		return retVal;
	}

	bool DrivingSegment::placeVehicleAtSpecifiedLength( Vehicle* vehicleToPlace )
	{
		bool retVal = false;
		//place the vehicle at the appropriate distance from the beginning of the road
		retVal = m_roadSpline->getPosition( vehicleToPlace->m_lengthTraveledAlongSpline,
											vehicleToPlace->m_currentLocation );
		if( retVal )
		{
			vehicleToPlace->m_previousLane = vehicleToPlace->m_currentLane;
			vehicleToPlace->m_currentLane = this;

			if( vehicleToPlace->m_previousLane )
			{
				vehicleToPlace->m_previousLane->removeVehicle( vehicleToPlace );
			}

			m_roadSpline->m_vehiclesOnSpline.push_back( vehicleToPlace );
			m_dirty = true;
		}

		return retVal;
	}

	bool DrivingSegment::removeVehicle( Vehicle* vehicleExitingLane )
	{
		//search for the vehicle and then remove it from the container
		for( std::vector< Vehicle* >::iterator vehicleInLane = m_roadSpline->m_vehiclesOnSpline.begin();
			vehicleInLane != m_roadSpline->m_vehiclesOnSpline.end(); )
		{
			if( ( *vehicleInLane ) == vehicleExitingLane )
			{
				//we have found the vehicle in the lane and need to remove it
				vehicleInLane = m_roadSpline->m_vehiclesOnSpline.erase( vehicleInLane );
				m_dirty = true;
				return true;
			}
			else
			{
				++vehicleInLane;
			}
		}

		//at this point we have traversed all of the items in the vector and did not find the vehicle exiting the lane
		return false;
	}

	Vector3 DrivingSegment::getStartingLocation() const
	{
		assert( m_roadSpline->getLength() != 0.f );
		
		Vector3 out_Position;
		m_roadSpline->getPosition(0.f, out_Position );

		return out_Position;
	}

	float DrivingSegment::getLength()
	{
		return m_roadSpline->getLength();
	}

	Vector3 DrivingSegment::getEndLocation() const
	{
		Vector3 out_Position;
		m_roadSpline->getPosition( m_roadSpline->getLength(), out_Position );
		
		return out_Position; 
	}

	Intersection* DrivingSegment::getEndIntersection()
	{
		return nullptr;
	}

	int DrivingSegment::getID()
	{
		return -1;
	}

	Vehicle* DrivingSegment::getVehicleInFrontOf( Vehicle* vehicleOnBack, float startingPoint )
	{
		return m_roadSpline->getVehicleInFrontOf( vehicleOnBack, startingPoint );
	}

	bool DrivingSegment::getDirectionAt( float in_pointInSpline, Vector3& out_direction )
	{
		return m_roadSpline->getDirection( in_pointInSpline, out_direction );
	}

	bool DrivingSegment::isBusy()
	{
		return !m_roadSpline->m_vehiclesOnSpline.empty();
	}

	bool DrivingSegment::isFull()
	{
		for( std::vector< Vehicle* >::iterator currentVehicle = m_roadSpline->m_vehiclesOnSpline.begin();
			currentVehicle != m_roadSpline->m_vehiclesOnSpline.end(); ++currentVehicle )
		{
			if( (*currentVehicle)->m_lengthTraveledAlongSpline < 50.f && (*currentVehicle)->m_speed < 1.f )
			{
				return true;
			}
		}
		
		return false;
	}

}