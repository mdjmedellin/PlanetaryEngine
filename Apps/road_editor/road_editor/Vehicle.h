#pragma once
#ifndef VEHICLE_H
#define VEHICLE_H

//////////////////////////////////////////////////////////////////////////
//===========================INCLUDES===============================
#include "Vector3.hpp"
#include "Rgba.h"
//==================================================================
//////////////////////////////////////////////////////////////////////////

namespace gh
{
	//////////////////////////////////////////////////////////////////////////
	//======================FORWARD DECLARATIONS====================
	class MatrixStack;
	class RoadSystem;
	class Road;
	class LaneNode;
	class RoadSpline;
	class Intersection;
	class DrivingLane;
	class RoadPath;
	class DrivingSegment;
	//==============================================================
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//================================VEHICLE=================================
	class Vehicle
	{
	public:
		Vehicle( float speed, Rgba vehicleColor = Rgba::WHITE );
		void setRoadSystemPtr( RoadSystem* roadSystem );
		void updateSimulation( double deltaTime );
		void setLocation( Vector3 vehicleLocation );
		void setVelocity( Vector3 vehicleVelocity );
		void setSteeringAngle( float steeringAngle );
		void render( MatrixStack& currentMatrixStack );
		DrivingSegment* getCurrentLane();
		DrivingSegment* peekNextLane();
		void setAsQueued();
		void placeRandomlyInWorld();
		void resetPointerToNextvehicle();

		DrivingSegment* m_previousLane;
		DrivingSegment* m_currentLane;
		float m_lengthTraveledAlongSpline;
		Road* m_roadDrivingOn;
		Intersection* m_nextIntersection;
		bool m_travelingAlongIntersection;
		Vector3 m_currentLocation;
		bool bReachedDestination;
		int id;
		float m_speed;

	private:
		void updateSpeed( double deltaTime );
		void updateVelocityVerlet( double deltaTime );
		void updateLocation( double deltaTime );
		float getDistanceToCollideWithVehicleInFront();
		void getNextVehicleInFront();
		void calculateDeceleration( float desiredSpeed, float distanceToDesiredSpeed );

		//==========================DATA MEMBERS====================
		float m_acceleration;
		//float m_speed;
		float m_bufferSpace;
		RoadSystem* m_roadSystem;
		Vector3 m_direction;
		Vector3 m_alignmentDirection;
		float m_alignmentDistance;
		bool m_aligningToTrack;
		bool m_decelerating;
		float m_desiredSpeed;
		float Max_Speed;
		Vehicle* m_vehicleInFrontOfMe;
		Rgba m_color;
		//int id;
		bool m_isQueued;
		RoadPath* m_roadPath;
		bool m_dirty;
		//===========================================================
	};
	//========================================================================
	//////////////////////////////////////////////////////////////////////////

	static int m_num = 0;
}

#endif