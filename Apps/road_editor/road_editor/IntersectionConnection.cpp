#include "stdafx.h"
#include "IntersectionConnection.h"
#include "DrivingLane.h"
#include "IntersectionDrivingSegment.h"
#include "vehicle.h"

namespace gh
{
	void IntersectionConnection::renderIntersection( MatrixStack& currentMatrixStack, float scale, const Rgba& color )
	{
		m_drivingSegment->render( currentMatrixStack, scale, color );
	}

	DrivingLane* IntersectionConnection::getOutgoingLane()
	{
		return m_outgoingDrivingLane;
	}

	DrivingLane* IntersectionConnection::getIncomingLane()
	{
		return m_incomingDrivingLane;
	}

	IntersectionConnection::IntersectionConnection( DrivingLane* incomingDrivingLane, DrivingLane* outgoingDrivingLane )
		:	m_drivingSegment( new IntersectionDrivingSegment( incomingDrivingLane, outgoingDrivingLane ) )
		,	m_incomingDrivingLane( incomingDrivingLane )
		,	m_outgoingDrivingLane( outgoingDrivingLane )
	{}

	DrivingSegment* IntersectionConnection::getDrivingSegment()
	{
		return m_drivingSegment;
	}

}