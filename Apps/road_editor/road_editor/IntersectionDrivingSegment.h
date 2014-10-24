#pragma once
#ifndef INTERSECTION_DRIVING_SEGMENT_H
#define INTERSECTION_DRIVING_SEGMENT_H

#include "DrivingSegment.h"

namespace gh
{
	class DrivingLane;


	class IntersectionDrivingSegment : public DrivingSegment
	{
	public:
		IntersectionDrivingSegment( DrivingLane* incomingDrivingLane, DrivingLane* outgoingDrivingLane );
		~IntersectionDrivingSegment();

		virtual void update( double	deltaTime );

	private:
		
	};
}

#endif