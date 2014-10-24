#pragma once
#ifndef INTERSECTION_CONNECTION_H
#define INTERSECTION_CONNECTION_H

//====================================INCLUDES==========================================
#include "IntersectionSpline.h"
//======================================================================================

namespace gh
{
	//=========================================FORWARD DECLARATION========================================
	class DrivingLane;
	class Vehicle;
	class MatrixStack;
	class Rgba;
	class IntersectionDrivingSegment;
	class DrivingSegment;
	//====================================================================================================

	//=======================================CLASS DECLARATION============================================
	class IntersectionConnection
	{
	public:
		IntersectionConnection( DrivingLane* incomingDrivingLane, DrivingLane* outgoingDrivingLane );
		void renderIntersection( MatrixStack& currentMatrixStack, const Rgba& color );
		DrivingLane* getOutgoingLane();
		DrivingLane* getIncomingLane();
		DrivingSegment* getDrivingSegment();
		//IntersectionSpline* getRoadSpline();

		int m_informationIndex;

	private:
		DrivingLane* m_incomingDrivingLane;
		DrivingLane* m_outgoingDrivingLane;
		IntersectionDrivingSegment* m_drivingSegment;
		//IntersectionSpline m_intersectionRoadSpline;
	};
	//=====================================================================================================
}

#endif