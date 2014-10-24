#pragma once
#ifndef INTERSECTION_SPLINE
#define INTERSECTION_SPLINE

//=====================================INCLUDES==========================================
#include "RoadSpline.h"
//=======================================================================================

namespace gh
{
	class IntersectionSpline : public RoadSpline
	{
	public:
		IntersectionSpline();
		virtual bool getPosition( float in_pointInSpline, Vector3& out_position );
		virtual bool getDirection( float in_pointInSpline, Vector3& out_direction );
		virtual float getRandomDistanceWithin();
		virtual float getLength();
		virtual void render( MatrixStack& currentMatrixStack, const Rgba& color = Rgba::WHITE );
	};
}

#endif