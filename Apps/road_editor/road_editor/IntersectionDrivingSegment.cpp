#include "stdafx.h"
#include "IntersectionDrivingSegment.h"
#include "DrivingLane.h"
#include "IntersectionSpline.h"

namespace gh
{
	IntersectionDrivingSegment::IntersectionDrivingSegment( DrivingLane* incomingDrivingLane, DrivingLane* outgoingDrivingLane )
		:	DrivingSegment()
	{
		//create an intersection road spline
		delete m_roadSpline;
		m_roadSpline = new IntersectionSpline();
		//connect the last two driving nodes of the incoming Driving Lane with the first two driving nodes of
		//the outgoing driving lane
		LaneNode* tempLaneNode1 = nullptr;
		LaneNode* tempLaneNode2 = nullptr;

		incomingDrivingLane->getLastTwoLaneNodes( tempLaneNode1, tempLaneNode2 );
		m_roadSpline->pushLaneNode( tempLaneNode1 );
		m_roadSpline->pushLaneNode( tempLaneNode2 );

		outgoingDrivingLane->getFirstTwoLaneNodes( tempLaneNode1, tempLaneNode2 );
		m_roadSpline->pushLaneNode( tempLaneNode1 );
		m_roadSpline->pushLaneNode( tempLaneNode2 );

		m_roadSpline->createSpline();

		//tell the outgoing driving lane about the connecting lane
		outgoingDrivingLane->setConnectingLane( incomingDrivingLane );
	}

	IntersectionDrivingSegment::~IntersectionDrivingSegment()
	{}

	void IntersectionDrivingSegment::update( double deltaTime )
	{
		int x = 1;
		DrivingSegment::update( deltaTime );
	}

}