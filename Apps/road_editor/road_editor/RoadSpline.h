#pragma once
#ifndef ROAD_SPLINE_H
#define ROAD_SPLINE_H

//===================================INCLUDES==================================
#include <vector>
#include "Vector3.hpp"
#include "Rgba.h"
//=============================================================================

namespace gh
{

	//=================================STRUCTS===============================
	struct SplineNode
	{
		SplineNode()
			:	m_position( Vector3() )
			,	m_velocity( Vector3() )
			,	m_distanceToNextSplineNode( 0.f )
			,	m_inverseDistanceToNextSplineNode( 0.f )
		{}

		Vector3 m_position;
		Vector3 m_velocity;
		float m_distanceToNextSplineNode;
		float m_inverseDistanceToNextSplineNode;
	};
	//======================================================================


	//============================FORWARD DECLARATIONS=====================
	class LaneNode;
	class MatrixStack;
	class Vehicle;
	//=====================================================================


	//==========================CLASS DECLARATION===========================
	class RoadSpline
	{
	public:
		RoadSpline();
		Vehicle* getVehicleInFrontOf( Vehicle* vehicleOnBack, float startingPoint );
		void pushLaneNode( LaneNode* in_laneNode );

		virtual void createSpline();
		virtual void render( MatrixStack& currentMatrixStack, const Rgba& color = Rgba::WHITE );
		virtual bool getPosition( float in_pointInSpline, Vector3& out_position );
		virtual bool getDirection( float in_pointInSpline, Vector3& out_direction );
		virtual float getRandomDistanceWithin();
		virtual float getLength();

		std::vector< Vehicle* > m_vehiclesOnSpline;

	protected:
		float m_totalLength;
		std::vector< SplineNode > m_splineNodes;
		
	};
	//=======================================================================
}

#endif