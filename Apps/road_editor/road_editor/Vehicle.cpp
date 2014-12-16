#include "StdAfx.h"
#include "Vehicle.h"
#include "MatrixStack.h"
#include "Renderer.h"
#include "Vector4.h"
#include "RoadSystem.h"
#include "Road.h"
#include "LaneNode.h"
#include "RoadSpline.h"
#include "Intersection.h"
#include "DrivingLane.h"
#include "RoadPath.h"
#include <algorithm>

namespace
{
	const float vehicleWidth = 10.f;
	const float vehicleLength = 10.f;

	const Vector2 halfVehicleDimensions( vehicleWidth * .5f, vehicleLength * .5f );
	const AABB2 vehicleRenderQuad( -halfVehicleDimensions, halfVehicleDimensions );
}

namespace gh
{
	void Vehicle::updateSimulation( double deltaTime )
	{
		updateLocation( deltaTime );
		updateSpeed( deltaTime );
	}

	void Vehicle::updateLocation( double deltaTime )
	{
		if(m_goalLaneID < 0)
		{
			return;
		}

		float distanceToAdvance = m_speed * float( deltaTime ) + ( .5f * m_acceleration * float( deltaTime * deltaTime ) );

		//advance the vehicle along the spline
		m_lengthTraveledAlongSpline += distanceToAdvance;

		//check if we should transition roads/intersections
		if( m_lengthTraveledAlongSpline > m_currentLane->getLength() )
		{
			//transition the vehicle to its new corresponding road
			//check if the road is not a dead end
			if( m_nextIntersection )
			{
				//if the vehicle was currently traveling an intersection
				if( m_travelingAlongIntersection )
				{
					DrivingSegment* nextDrivingSegment = m_roadPath->getNextLaneInPath();

					if( nextDrivingSegment )
					{
						m_lengthTraveledAlongSpline -= m_currentLane->getLength();
						nextDrivingSegment->placeVehicleAtSpecifiedLength( this );

						m_travelingAlongIntersection = false;
					}
					else
					{
						/*m_currentLane = m_previousLane;*/
						m_lengthTraveledAlongSpline = m_currentLane->getLength();
					}
				}
				else
				{
					m_nextIntersection->attemptToCross( this );
				}
			}
			else
			{
				//in case we are in a dead end
				//we clamp the vehicle to that direction
				m_lengthTraveledAlongSpline = m_currentLane->getLength();
			}
		}

		if( !m_roadPath->peekNextLaneInPath() )
		{
			
			if( m_currentLane->getID() == m_goalLaneID )
			{
				bReachedDestination = true;
				//m_roadPath = m_roadSystem->calculatePathTo( "lane1_1_1", 0.f, m_currentLane );
			}
			else
			{
				m_roadPath = m_roadSystem->calculatePathTo( m_goalLaneID, 0.f, m_currentLane );
			}

			m_nextIntersection = m_currentLane->getEndIntersection();
		}
		
		if( m_aligningToTrack && m_alignmentDistance > m_lengthTraveledAlongSpline )
		{
			m_currentLocation += ( m_alignmentDirection * m_speed * float( deltaTime ) );
		}
		else
		{
			m_aligningToTrack = false;
			m_currentLane->getDirectionAt( m_lengthTraveledAlongSpline, m_direction );
			m_currentLocation += ( m_direction * m_speed *  float( deltaTime ) ) + ( m_direction * .5f * m_acceleration * float( deltaTime * deltaTime ) );
		}
	}


	//DistanceToCollision returns the distance that needs to be covered before the two vehicles collide
	float Vehicle::getDistanceToCollideWithVehicleInFront()
	{
		if( m_vehicleInFrontOfMe == nullptr )
		{
			return 999999.f;
		}

		float startPoint = m_lengthTraveledAlongSpline;
		DrivingSegment* currentLane = m_currentLane;
		RoadPath currentRoadPath = *m_roadPath;
		bool amICurrentlyInAnIntersection = m_travelingAlongIntersection;
		Intersection* currentIntersection = m_nextIntersection;
		float distanceBetweenCars = 0;
		bool foundCollisionPoint = false;

		//find the lane on which this vehicle is located
		while( startPoint > currentLane->getLength() && !foundCollisionPoint )
		{
			if( m_vehicleInFrontOfMe->m_currentLane == currentLane )
			{
				distanceBetweenCars += m_vehicleInFrontOfMe->m_lengthTraveledAlongSpline - startPoint;
				foundCollisionPoint = true;
			}
			else
			{
				startPoint -= currentLane->getLength();

				if( amICurrentlyInAnIntersection )
				{
					currentLane = currentRoadPath.getNextLaneInPath();
					if( currentLane )
					{
						amICurrentlyInAnIntersection = false;
					}
					else
					{
						return 999999.f;
					}
				}
				else
				{
					currentLane = currentIntersection->getCorrespondingDrivingSegment( currentLane, currentRoadPath.peekNextLaneInPath() );
					amICurrentlyInAnIntersection = true;

					if( currentLane == nullptr )
					{
						return 999999.f;
					}
				}
			}
		}

		//find the lane on which the vehicle in front of me is located and accumulate the distance between them
		// along the way
		while( !foundCollisionPoint )
		{
			if( currentLane == m_vehicleInFrontOfMe->m_currentLane )
			{
				distanceBetweenCars += m_vehicleInFrontOfMe->m_lengthTraveledAlongSpline - startPoint;
				return distanceBetweenCars;
			}
			else
			{
				distanceBetweenCars += currentLane->getLength();

				if( amICurrentlyInAnIntersection )
				{
					currentLane = currentRoadPath.getNextLaneInPath();
					if( currentLane )
					{
						amICurrentlyInAnIntersection = false;
					}
					else
					{
						return 999999.f;
					}
				}
				else
				{
					currentLane = currentIntersection->getCorrespondingDrivingSegment( currentLane, currentRoadPath.peekNextLaneInPath() );
					amICurrentlyInAnIntersection = true;

					if( currentLane == nullptr )
					{
						return 999999.f;
					}
				}
			}
		}

		return 999999.f;
	}

	void Vehicle::getNextVehicleInFront()
	{
		float startPoint = m_lengthTraveledAlongSpline;
		DrivingSegment* currentLane = m_currentLane;
		RoadPath currentRoadPath = *m_roadPath;
		bool amICurrentlyInAnIntersection = m_travelingAlongIntersection;
		Intersection* currentIntersection = m_nextIntersection;
		float distanceBetweenCars = 0;
		
		//check if the vehicle is still on the current driving segment
		bool continueSearchingForStartPoint = false;
		while( startPoint > currentLane->getLength() && !continueSearchingForStartPoint )
		{
			startPoint -= currentLane->getLength();

			if( amICurrentlyInAnIntersection )
			{
				currentLane = currentRoadPath.getNextLaneInPath();
				if( currentLane )
				{
					amICurrentlyInAnIntersection = false;
				}
				else
				{
					//at this point we are pass what our path indicates
					continueSearchingForStartPoint = true;
				}
			}
			else
			{
				currentLane = currentIntersection->getCorrespondingDrivingSegment( currentLane, currentRoadPath.peekNextLaneInPath() );
				amICurrentlyInAnIntersection = true;

				if( currentLane == nullptr )
				{
					//at this point we are at a point were we cannot reach the next point. We must re-path
					continueSearchingForStartPoint = true;
				}
			}
		}

		//if we arrive at this point then
		// 1.- we have moved to the starting point were we must look for a starting position
		// 2.- we traverse the rest of the path for the next vehicle in front that is on our path
		while( m_vehicleInFrontOfMe == nullptr && currentLane != nullptr )
		{
			m_vehicleInFrontOfMe = currentLane->getVehicleInFrontOf( this, startPoint );

			if( m_vehicleInFrontOfMe )
			{
				//we have found the vehicle in front of the car
				continue;
			}
			else
			{
				startPoint = 0.f;
				//move to the next section of our road
				if( amICurrentlyInAnIntersection )
				{
					currentLane = currentRoadPath.getNextLaneInPath();
					if( currentLane )
					{
						amICurrentlyInAnIntersection = false;
					}
					else
					{
						//we are at the end of our path?
						break;
					}
				}
				else
				{
					currentLane = currentIntersection->getCorrespondingDrivingSegment( currentLane, currentRoadPath.peekNextLaneInPath() );
					amICurrentlyInAnIntersection = true;

					if( currentLane == nullptr )
					{
						//we are at a point were our path led us to an intersection that does not lead us to our destination
						//must re-path
						break;
					}
				}
			}
		}
	}

	void Vehicle::updateSpeed( double deltaTime )
	{
		float distanceToCollideToVehicleInFront = 999999.f;
		float distanceToSignalLight = 999999.f;
		bool stopOnSignalLight = false;
		bool stopForVehicleOnFront = false;

		//have the lane update the vehicles when updating
		if( m_dirty )
		{
			//look for the vehicle in front of me
			getNextVehicleInFront();

			if( m_vehicleInFrontOfMe && m_vehicleInFrontOfMe->m_vehicleInFrontOfMe == this )
			{
				m_currentLane->m_dirty = true;
			}
			m_dirty = false;
		}

		if( m_vehicleInFrontOfMe )
		{
			/*float currentLength = m_lengthTraveledAlongSpline;*/
			/*distanceToCollideToVehicleOnFront = DistanceToCollision( currentLength, this, m_vehicleInFrontOfMe )*/;
			distanceToCollideToVehicleInFront = getDistanceToCollideWithVehicleInFront();

			if( distanceToCollideToVehicleInFront <= 60.f )
			{
				stopForVehicleOnFront = true;
			}
		}

		//am i able to see the stop light
		distanceToSignalLight = m_currentLane->getLength() - m_lengthTraveledAlongSpline;
		if( distanceToSignalLight <= 25.f )
		{
			//am I traveling on a road
			if( !m_travelingAlongIntersection )
			{
				IntersectionCondition testCondition = m_nextIntersection->getIntersectionCondition( this );
				//check the intersection
				switch( m_nextIntersection->getIntersectionCondition( this ) )
				{
				case RED_SIGNAL_LIGHT:
					{
						stopOnSignalLight = true;
						break;
					}

				default:
					{}
				}

				//check if the intersection is currently busy
				if( stopOnSignalLight == false )
				{
					stopOnSignalLight = m_nextIntersection->checkIfIntersectionIsBusy( this );
				}

				//check if the lane I want to go to is full
				if( stopOnSignalLight == false )
				{
					if( m_roadPath->peekNextLaneInPath() && m_roadPath->peekNextLaneInPath()->isFull() )
					{
						stopOnSignalLight = true;
					}
				}
			}
		}

		if( stopForVehicleOnFront && stopOnSignalLight && !m_travelingAlongIntersection )
		{
			//determine which is closest
			if( distanceToCollideToVehicleInFront < distanceToSignalLight )
			{
				if( distanceToCollideToVehicleInFront - m_bufferSpace >= 0.f )
				{
					calculateDeceleration( m_vehicleInFrontOfMe->m_speed, distanceToCollideToVehicleInFront - m_bufferSpace );	
					m_desiredSpeed = m_vehicleInFrontOfMe->m_speed;
				}
				else
				{
					calculateDeceleration( 0.f, 0.0000000001f );
					m_desiredSpeed = 0.f;
				}
				m_decelerating = true;
			}
			else
			{
				calculateDeceleration( 0.f, distanceToSignalLight );
				m_decelerating = true;
				m_desiredSpeed = 0.f;
			}
		}
		else if( stopOnSignalLight )
		{
			//stop according to the signal light
			calculateDeceleration( 0.f, distanceToSignalLight );
			m_decelerating = true;
			m_desiredSpeed = 0.f;
		}
		else if( stopForVehicleOnFront )
		{
			//stop according to the vehicle in front
			if( distanceToCollideToVehicleInFront - m_bufferSpace >= 0.f )
			{
				calculateDeceleration( m_vehicleInFrontOfMe->m_speed, distanceToCollideToVehicleInFront - m_bufferSpace );	
				m_desiredSpeed = m_vehicleInFrontOfMe->m_speed;
			}
			else
			{
				calculateDeceleration( 0.f, 0.0000000001f );
				m_desiredSpeed = 0.f;
			}
			m_decelerating = true;
		}
		else
		{
			//at this point you should accelerate
			float interpolationVal = RandZeroToOne();
			m_acceleration = ( 10.f * ( 1.f - interpolationVal ) + ( 25.f * interpolationVal ) );

			if( m_speed > Max_Speed )
			{
				m_acceleration = -m_acceleration;
			}

			m_decelerating = false;
		}

		m_speed = m_speed + m_acceleration * float( deltaTime );
		if( m_speed > 0.00005f && id == 3 )
		{
			int x = 1;
		}
		if( id == 3 )
		{
			int y = 1;
		}
	}

	void Vehicle::calculateDeceleration( float desiredSpeed, float distanceToDesiredSpeed )
	{
		if( distanceToDesiredSpeed > 0.001f || -0.001f > distanceToDesiredSpeed )
		{
			m_acceleration = ( -( m_speed * m_speed ) ) / ( 2.f * distanceToDesiredSpeed );
		}
		else
		{
			m_acceleration = 0.f;
			m_speed = m_desiredSpeed;
		}
	}

	Vehicle::Vehicle( float speed, Rgba carColor /* Rgba::WHITE */ )
		:	m_currentLocation( Vector3() )
		,	m_roadDrivingOn( nullptr )
		,	m_roadSystem( nullptr )
		,	m_acceleration( 0.f )
		,	m_speed( 0.f )
		,	m_nextIntersection( nullptr )
		,	m_travelingAlongIntersection( false )
		,	m_aligningToTrack( false )
		,	m_alignmentDistance( 0.f )
		,	m_decelerating( false )
		,	m_desiredSpeed( 0.f )
		,	m_vehicleInFrontOfMe( nullptr )
		,	m_color( carColor )
		,	m_bufferSpace( 40.f )
		,	id( ++m_num )
		,	bReachedDestination( false )
		,	m_dirty( true )
	{
		Max_Speed = speed;
	}

	void Vehicle::render( MatrixStack& currentMatrixStack, float scale )
	{
		currentMatrixStack.PushMatrix();
		currentMatrixStack.Translate( m_currentLocation );
		
		glLoadMatrixf( currentMatrixStack.GetTopMatrixFV() );
		theRenderer.renderQuad( vehicleRenderQuad, m_color.getVector4(), nullptr );
		char buffer[ 256 ];
		sprintf( buffer, "%i", id );
		theRenderer.drawText( buffer, "test", halfVehicleDimensions.y * 3, vehicleRenderQuad, BOX_ALIGN | TEXT_ALIGN_CENTERX | TEXT_ALIGN_CENTERY , Rgba::WHITE );
		theRenderer.disableAllTextures();
		currentMatrixStack.PopMatrix();
	}

	void Vehicle::setRoadSystemPtr( RoadSystem* roadSystem )
	{
		m_roadSystem = roadSystem;
		placeRandomlyInWorld();
	}

	void Vehicle::placeRandomlyInWorld()
	{
		m_currentLane = nullptr;
		DrivingLane* goalLane = nullptr;
		m_roadPath = nullptr;

		do{
			m_currentLane = m_roadSystem->getRandomLane();
			assert( m_currentLane );
			
			if( !m_currentLane->placeVehicleRandomly( this ) )
			{
				m_roadPath = nullptr;
				continue;
			}

			goalLane = m_roadSystem->getRandomLane();
			m_roadPath = m_roadSystem->calculatePathTo(goalLane->getID(), 1.f, m_currentLane);
		}while( m_roadPath == nullptr || m_roadPath->peekNextLaneInPath() == nullptr );
	}

	DrivingSegment* Vehicle::getCurrentLane()
	{
		return m_currentLane;
	}

	DrivingSegment* Vehicle::peekNextLane()
	{
		DrivingSegment* nextDesiredLane = m_roadPath->peekNextLaneInPath();

		return nextDesiredLane;
	}

	void Vehicle::setAsQueued()
	{
		m_isQueued = true;
	}

	void Vehicle::resetPointerToNextvehicle()
	{
		m_vehicleInFrontOfMe = nullptr;
		m_dirty = true;
	}
}