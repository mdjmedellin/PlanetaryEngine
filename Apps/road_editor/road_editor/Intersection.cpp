#include "StdAfx.h"
#include "Intersection.h"
#include "IntersectionConnection.h"
#include "DrivingLane.h"
#include "Vehicle.h"
#include "Rgba.h"

namespace gh
{
	bool Intersection::attemptToCross( Vehicle* vehicleAttemptingToCross )
	{
		//check if the lane the vehicle is in connects to the intersection
		IncomingToOutgoingLaneMap::iterator incomingToOutgoingLane = m_intersectionConnectionsMap.find( vehicleAttemptingToCross->getCurrentLane() );
		if( incomingToOutgoingLane != m_intersectionConnectionsMap.end() )
		{
			//check if the next lane connects to the current lane via this intersection
			OutgoingLanetoIntersectionMap::iterator outgoingLaneToIntersection;
			outgoingLaneToIntersection = (*incomingToOutgoingLane).second.find( vehicleAttemptingToCross->peekNextLane() );

			if( outgoingLaneToIntersection != (*incomingToOutgoingLane).second.end() )
			{
				//check the type of intersection
				int intersectionInformationIndex = (*outgoingLaneToIntersection).second.first;
				IntersectionInformation* currentInformation = &m_synchronizedIntersectionsInformation[ intersectionInformationIndex ];

				switch( currentInformation->m_type )
				{
				case ALWAYS_STOP:
					{
						/*//if it always has to stop, we add the vehicle to the queue
						m_vehicleQueue.push_back(vehicleAttemptingToCross);
						vehicleAttemptingToCross->setAsQueued();
						return false;*/
						vehicleAttemptingToCross->m_lengthTraveledAlongSpline -= vehicleAttemptingToCross->getCurrentLane()->getLength();
						
						if( (*outgoingLaneToIntersection).second.second->getDrivingSegment()->placeVehicleAtSpecifiedLength( vehicleAttemptingToCross ) )
						{
							vehicleAttemptingToCross->m_travelingAlongIntersection = true;
							return true;
						}
						else
						{
							vehicleAttemptingToCross->m_lengthTraveledAlongSpline = vehicleAttemptingToCross->getCurrentLane()->getLength();
							return false;
						}
						
					}
				case SIGNAL_LIGHT:
					{
						switch( currentInformation->m_lightColor )
						{
						case GREEN_SIGNAL_LIGHT:
						case YELLOW_SIGNAL_LIGHT:
							{
								vehicleAttemptingToCross->m_lengthTraveledAlongSpline -= vehicleAttemptingToCross->getCurrentLane()->getLength();

								if( (*outgoingLaneToIntersection).second.second->getDrivingSegment()->placeVehicleAtSpecifiedLength( vehicleAttemptingToCross ) )
								{
									vehicleAttemptingToCross->m_travelingAlongIntersection = true;
									return true;
								}
								else
								{
									vehicleAttemptingToCross->m_lengthTraveledAlongSpline = vehicleAttemptingToCross->getCurrentLane()->getLength();
									return false;
								}
								break;
							}
						default:
							{}
						}

						return true;
					}
				default:
					{
						vehicleAttemptingToCross->m_lengthTraveledAlongSpline -= vehicleAttemptingToCross->getCurrentLane()->getLength();

						if( (*outgoingLaneToIntersection).second.second->getDrivingSegment()->placeVehicleAtSpecifiedLength( vehicleAttemptingToCross ) )
						{
							vehicleAttemptingToCross->m_travelingAlongIntersection = true;
							return true;
						}
						else
						{
							vehicleAttemptingToCross->m_lengthTraveledAlongSpline = vehicleAttemptingToCross->getCurrentLane()->getLength();
							return false;
						}
					}
				}
			}
			else
			{
				return false;
			}
		}

		return false;
	}

	DrivingSegment* Intersection::getCorrespondingDrivingSegment( DrivingSegment* incomingDrivingLane, DrivingSegment* outgoingDrivingLane )
	{
		IncomingToOutgoingLaneMap::iterator incomingToOutgoingLane = m_intersectionConnectionsMap.find( incomingDrivingLane );
		
		if( incomingToOutgoingLane != m_intersectionConnectionsMap.end() )
		{
			OutgoingLanetoIntersectionMap::iterator outgoingLaneToIntersection;
			outgoingLaneToIntersection = (*incomingToOutgoingLane).second.find( outgoingDrivingLane );

			if( outgoingLaneToIntersection != (*incomingToOutgoingLane).second.end() )
			{
				return (*outgoingLaneToIntersection).second.second->getDrivingSegment();
			}
			else
			{
				return nullptr;
			}
		}

		return nullptr;
	}


	void Intersection::getConnectingLanes( std::vector< DrivingSegment* >& out_connectingLanes, DrivingSegment* incomingLane )
	{
		IncomingToOutgoingLaneMap::iterator foundOutgoingLanesMap = m_intersectionConnectionsMap.find( incomingLane );

		if( foundOutgoingLanesMap != m_intersectionConnectionsMap.end() )
		{
			OutgoingLanetoIntersectionMap::iterator currentOutgoingLane = foundOutgoingLanesMap->second.begin();

			for( ; currentOutgoingLane != foundOutgoingLanesMap->second.end(); ++currentOutgoingLane )
			{
				out_connectingLanes.push_back( currentOutgoingLane->first );
			}
		}
	}

	void Intersection::addIntersectionConnections( std::vector< IntersectionConnection* > synchronizedConnections,
		IntersectionType typeOfIntersection /*= NEVER_STOP */, float greenLightDuration /*= 0.f*/, float yellowLightDuration /*= 0.f */ )
	{
		if( typeOfIntersection == SIGNAL_LIGHT )
		{
			++m_numOfStopLights;
		}

		//create the intersection information that is to be shared by all the connections
		IntersectionInformation currentSynchronizedConnectionsInformation( typeOfIntersection, greenLightDuration, yellowLightDuration );
		m_synchronizedIntersectionsInformation.push_back( currentSynchronizedConnectionsInformation );

		//for every connection, map it into the incoming lane
		for( std::vector< IntersectionConnection* >::iterator currentIntersectionConnection = synchronizedConnections.begin();
			currentIntersectionConnection != synchronizedConnections.end(); ++currentIntersectionConnection )
		{
			m_intersectionConnections.push_back( ( *currentIntersectionConnection ) );
			m_intersectionConnectionsMap[ (*currentIntersectionConnection)->getIncomingLane() ][ (*currentIntersectionConnection)->getOutgoingLane() ] = IntersectionInformationPair( m_synchronizedIntersectionsInformation.size()-1, (* currentIntersectionConnection ) );
			
			(*currentIntersectionConnection)->m_informationIndex = m_synchronizedIntersectionsInformation.size()-1;
		
			//notify all incoming lanes about this intersection
			(*currentIntersectionConnection)->getIncomingLane()->m_intersection = this;
		}

		//check if this is the first stop light to add to this intersection and turn it on
		if( m_synchronizedIntersectionsInformation.size() == 1 && m_synchronizedIntersectionsInformation[ m_currentSignalLight ].m_type == SIGNAL_LIGHT )
		{
			m_synchronizedIntersectionsInformation[ m_currentSignalLight ].m_lightColor = GREEN_SIGNAL_LIGHT;
		}
	}

	void Intersection::getAllOutgoingLanes( std::vector< DrivingSegment* >& out_outgoingLanes )
	{
		for( IncomingToOutgoingLaneMap::iterator currentIncomingLane = m_intersectionConnectionsMap.begin();
			currentIncomingLane != m_intersectionConnectionsMap.end(); ++currentIncomingLane )
		{
			for( OutgoingLanetoIntersectionMap::iterator currentOutgoingLane = currentIncomingLane->second.begin();
				currentOutgoingLane != currentIncomingLane->second.end(); ++currentOutgoingLane )
			{
				out_outgoingLanes.push_back( currentIncomingLane->first );
			}
		}
	}

	void Intersection::render( MatrixStack& currentMatrixStack, float scale )
	{
		for( std::vector< IntersectionConnection* >::iterator currentIntersectionConnection = m_intersectionConnections.begin();
			currentIntersectionConnection != m_intersectionConnections.end(); ++currentIntersectionConnection )
		{
			switch ( m_synchronizedIntersectionsInformation[ ( *currentIntersectionConnection )->m_informationIndex ].m_lightColor )
			{
			case RED_SIGNAL_LIGHT:
				{
					(*currentIntersectionConnection)->renderIntersection(currentMatrixStack, scale, Rgba::RED );
					break;
				}
			case YELLOW_SIGNAL_LIGHT:
				{
					(*currentIntersectionConnection)->renderIntersection(currentMatrixStack, scale, Rgba( 1.f, 1.f, 0.f ).m_RGBA );
					break;
				}
			case GREEN_SIGNAL_LIGHT:
				{
					(*currentIntersectionConnection)->renderIntersection(currentMatrixStack, scale, Rgba::GREEN );
					break;
				}
			}
			
		}
	}

	IntersectionCondition Intersection::getIntersectionCondition( Vehicle* incomingVehicle )
	{
		IncomingToOutgoingLaneMap::iterator incomingToOutgoingLane = m_intersectionConnectionsMap.find( incomingVehicle->getCurrentLane() );
		
		if( incomingToOutgoingLane != m_intersectionConnectionsMap.end() )
		{
			OutgoingLanetoIntersectionMap::iterator outgoingLaneToIntersection;
			outgoingLaneToIntersection = (*incomingToOutgoingLane).second.find( incomingVehicle->peekNextLane() );

			if( outgoingLaneToIntersection != (*incomingToOutgoingLane).second.end() )
			{
				int intersectionInformationIndex = (*outgoingLaneToIntersection).second.first;

				//check the type of intersection
				IntersectionInformation* currentInformation = &m_synchronizedIntersectionsInformation[ intersectionInformationIndex ];
				switch( currentInformation->m_type )
				{
				case ALWAYS_STOP:
					{
						//if it always has to stop, we add the vehicle to the queue
						m_vehicleQueue.push_back( incomingVehicle );
						incomingVehicle->setAsQueued();
						return STOP_INTERSECTION_CONDITION;
					}
				case SIGNAL_LIGHT:
					{
						//if it is on a signal light, we only return true if the signal light is green
						//for now lets assume signal lights are always valid
						return currentInformation->m_lightColor;
					}
				default:
					{
						return GO_INTERSECTION_CONDITION;
					}
				}
			}
			else
			{
				return ERROR_CONNECTION_NOT_FOUND;
			}
		}

		return ERROR_INVALID_INCOMING_LANE;
	}

	void Intersection::update( double deltaTime )
	{
		//check if this intersection has at least one stop light
		if( m_numOfStopLights == 0 )
		{
			return;
		}

		m_timeSpentOnCurrentSignal += float( deltaTime );

		//go to the appropriate signal
		while( m_timeSpentOnCurrentSignal > m_synchronizedIntersectionsInformation[ m_currentSignalLight ].getTotalLightTime() )
		{
			m_timeSpentOnCurrentSignal -= m_synchronizedIntersectionsInformation[ m_currentSignalLight ].getTotalLightTime();
			m_synchronizedIntersectionsInformation[ m_currentSignalLight ].m_lightColor = RED_SIGNAL_LIGHT;

			++m_currentSignalLight;
			if( m_currentSignalLight >= m_synchronizedIntersectionsInformation.size() )
			{
				m_currentSignalLight = 0;
			}
			m_synchronizedIntersectionsInformation[ m_currentSignalLight ].m_lightColor = GREEN_SIGNAL_LIGHT;
		}

		if( m_timeSpentOnCurrentSignal > m_synchronizedIntersectionsInformation[ m_currentSignalLight ].m_greenLightDuration )
		{
			m_synchronizedIntersectionsInformation[ m_currentSignalLight ].m_lightColor = YELLOW_SIGNAL_LIGHT;
		}

		//update the intersection connections
		for( std::vector< IntersectionConnection* >::iterator currentConnection = m_intersectionConnections.begin();
			currentConnection != m_intersectionConnections.end(); ++currentConnection )
		{
			(*currentConnection)->getDrivingSegment()->update(	deltaTime );
		}
	}

	Intersection::Intersection()
		:	m_currentSignalLight( 0 )
		,	m_timeSpentOnCurrentSignal( 0.f )
		,	m_numOfStopLights( 0 )
	{}

	bool Intersection::checkIfIntersectionIsBusy( Vehicle* vehicleAttemptingToCross )
	{
		for( std::vector< IntersectionConnection* >::iterator currentConnection = m_intersectionConnections.begin();
			currentConnection != m_intersectionConnections.end(); ++currentConnection )
		{
			if( (*currentConnection)->getOutgoingLane() == vehicleAttemptingToCross->peekNextLane() )
			{
				if( (*currentConnection)->getIncomingLane() == vehicleAttemptingToCross->getCurrentLane() )
				{
					continue;
				}
				else
				{
					if( (*currentConnection)->getDrivingSegment()->isBusy() )
					{
						return true;
					}
				}
			}
		}

		return false;
	}

}