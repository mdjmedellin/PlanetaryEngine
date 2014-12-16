#pragma once
#ifndef INTERSECTION_H
#define INTERSECTION_H

//====================================INCLUDES==============================================================
#include <vector>
#include <map>
#include <deque>
//==========================================================================================================

namespace gh
{
	//=========================================FORWARD DECLARATION==========================================
	class IntersectionConnection;
	class Vehicle;
	class MatrixStack;
	class DrivingLane;
	class DrivingSegment;
	class RoadSpline;
	//=======================================================================================================

	//=================================TYPEDEFS, ENUMERATIONS, AND STRUCTS===================================
	typedef std::pair< int, IntersectionConnection* > IntersectionInformationPair;
	typedef std::map< DrivingSegment*, std::map< DrivingSegment*, IntersectionInformationPair > > IncomingToOutgoingLaneMap;
	typedef std::map< DrivingSegment*, IntersectionInformationPair > OutgoingLanetoIntersectionMap; 


	enum IntersectionType{ NEVER_STOP, ALWAYS_STOP, SIGNAL_LIGHT };
	enum IntersectionCondition{ GREEN_SIGNAL_LIGHT, YELLOW_SIGNAL_LIGHT, RED_SIGNAL_LIGHT, STOP_INTERSECTION_CONDITION,
								GO_INTERSECTION_CONDITION, ERROR_CONNECTION_NOT_FOUND, ERROR_INVALID_INCOMING_LANE };


	struct IntersectionInformation
	{
		float m_greenLightDuration;
		float m_yellowLightDuration;
		IntersectionType m_type;
		IntersectionCondition m_lightColor;
		
		IntersectionInformation ( IntersectionType type, float greenLightDuration = 0.f, float yellowLightDuration = 0.f )
			:	m_type( type )
			,	m_greenLightDuration( greenLightDuration )
			,	m_yellowLightDuration( yellowLightDuration )
			,	m_lightColor( RED_SIGNAL_LIGHT )
		{}

		float getTotalLightTime()
		{
			return m_greenLightDuration + m_yellowLightDuration;
		}
	};
	//=============================================================================================================


	//=======================================CLASS DECLARATION=====================================================
	class Intersection
	{
	public:
		Intersection();
		void update( double deltaTime );
		void addIntersectionConnections( std::vector< IntersectionConnection* > synchronizedConnections,
			IntersectionType typeOfIntersection = NEVER_STOP, float greenLightDuration = 0.f, float yellowLightDuration = 0.f );
		bool attemptToCross( Vehicle* vehicleAttemptingToCross );
		bool checkIfIntersectionIsBusy( Vehicle* vehicleAttemptingToCross );
		DrivingSegment* getCorrespondingDrivingSegment( DrivingSegment* incomingDrivingLane, DrivingSegment* outgoingDrivingLane );
		IntersectionCondition getIntersectionCondition( Vehicle* incomingVehicle );
		void getConnectingLanes( std::vector< DrivingSegment* >& out_connectingLanes, DrivingSegment* incomingLane );
		void getAllOutgoingLanes( std::vector< DrivingSegment* >& out_outgoingLanes );
		void render( MatrixStack& currentMatrixStack, float scale );

	private:
		size_t m_currentSignalLight;
		float m_timeSpentOnCurrentSignal;
		std::vector< IntersectionInformation > m_synchronizedIntersectionsInformation;
		std::map< DrivingSegment*, std::map< DrivingSegment*, IntersectionInformationPair > > m_intersectionConnectionsMap;
		std::deque< Vehicle* > m_vehicleQueue;
		std::vector< IntersectionConnection* > m_intersectionConnections;
		size_t m_numOfStopLights;
	};
	//=============================================================================================================
}

#endif