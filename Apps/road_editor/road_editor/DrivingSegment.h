#pragma once
#ifndef DRIVING_SEGMENT_H
#define DRIVING_SEGMENT_H

//////////////////////////////////////////////////////////////////////////
//==========================INCLUDES====================================//
#include "Rgba.h"
#include "Vector3.hpp"
//////////////////////////////////////////////////////////////////////////


namespace gh
{
	//////////////////////////////////////////////////////////////////////////
	//=========================FORWARD DECLARATIONS=========================//
	class MatrixStack;
	class Vehicle;
	class RoadSpline;
	class Intersection;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//========================DRIVING SEGMENT===============================//
	class DrivingSegment
	{
	public:
		DrivingSegment();
		Vehicle* getVehicleInFrontOf( Vehicle* vehicleOnBack, float startingPoint );
		bool getDirectionAt( float in_pointInSpline, Vector3& out_direction );
		bool isBusy();
		bool isFull();
		virtual void update( double deltaTime );

		virtual ~DrivingSegment();
		virtual void render( MatrixStack& currentMatrixStack, float scale, const Rgba& color = Rgba::WHITE );
		virtual bool placeVehicleRandomly( Vehicle* vehicleToPlace );
		virtual bool placeVehicleAtSpecifiedLength( Vehicle* vehicleToPlace );
		virtual bool removeVehicle( Vehicle* vehicleExitingLane );
		virtual Vector3 getStartingLocation() const;
		virtual Vector3 getEndLocation() const;
		virtual float getLength();
		virtual Intersection* getEndIntersection();
		virtual int getID();

		bool m_dirty;

	protected:
		RoadSpline* m_roadSpline;
		
	};
	//////////////////////////////////////////////////////////////////////////
}


#endif