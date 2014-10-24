#ifndef TIMELINE_H
#define TIMELINE_H
#pragma once

#include "SpaceGameUtilities.hpp"
#include <list>

namespace gh
{
	class Timeline
	{
	public:
		void insertState( double timeInSeconds, const TimelineState& stateToInsert );
		TimelineState getState( double timeInSeconds ) const;

	private:
		void insertAfterCurrentTimeSlot( double timeToInputState, const TimelineState& stateToInput );
		void insertBeforeCurrentTimeSlot( double timeToInputState, const TimelineState& stateToInput );
		TimelineState getBestStateForwardInTime( double desiredInputStateTime ) const;
		TimelineState getBestStateBackwardInTime( double desiredInputStateTime ) const;
		TimelineState extrapolateState( std::list< std::pair< double, TimelineState > >::iterator currentState,
										double desiredStateTime ) const;
		Vector2 getLocalAcceleration( const Vector2& acceleration, float orientation ) const;
		TimelineState interpolateState( std::list< std::pair< double, TimelineState > >::iterator initialState,
										std::list< std::pair< double, TimelineState > >::iterator currentState,
										double desiredStateTime ) const;

		std::list< std::pair< double, TimelineState > > m_timeline;
		std::list< std::pair< double, TimelineState > >::iterator m_currentTimeSlot;
	};
}

#endif