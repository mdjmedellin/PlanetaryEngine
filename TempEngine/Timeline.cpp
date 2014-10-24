#include "Timeline.hpp"
#include "TimeUtility.h"
#include <utility>

namespace
{
	const double DRAG_COEFFICIENT = -0.025;
}

namespace gh
{
	void Timeline::insertState( double timeInSeconds, const TimelineState& stateToInsert )
	{
		//if the timeline is currently empty, we go ahead and insert
		if( m_timeline.empty() )
		{
			m_timeline.push_back( std::pair< double, TimelineState >( timeInSeconds, stateToInsert ) );
			m_currentTimeSlot = m_timeline.begin();
		}
		else
		{
			std::list< std::pair< double, InputState > >::iterator timeSlot;

			if( timeInSeconds > m_currentTimeSlot->first )
			{
				insertAfterCurrentTimeSlot( timeInSeconds, stateToInsert );
			}
			else if( timeInSeconds < m_currentTimeSlot->first )
			{
				insertBeforeCurrentTimeSlot( timeInSeconds, stateToInsert );
			}
			else
			{
				m_currentTimeSlot->second = stateToInsert;
			}
		}
	}

	void Timeline::insertAfterCurrentTimeSlot( double timeToInputState, const TimelineState& stateToInsert )
	{
		std::list< std::pair< double, TimelineState > >::iterator timeSlot;

		for( timeSlot = m_currentTimeSlot; timeSlot != m_timeline.end(); ++timeSlot )
		{
			if( timeSlot->first > timeToInputState )
			{
				break;
			}
		}

		if( timeSlot == m_timeline.end() )
		{
			//add it to the back
			m_timeline.push_back( std::pair< double, TimelineState >( timeToInputState, stateToInsert ) );
		}
		else
		{
			//timeSlot points to the position where this state should be inserted
			m_currentTimeSlot = m_timeline.insert( timeSlot, std::pair< double, TimelineState >( timeToInputState, stateToInsert ) );
		}
	}

	void Timeline::insertBeforeCurrentTimeSlot( double timeToInputState, const TimelineState& stateToInsert )
	{
		std::list< std::pair< double, TimelineState > >::iterator timeSlot;

		//we are supposed to insert this state in the past
		bool outOfBounds = false;
		for( timeSlot = m_currentTimeSlot; !outOfBounds; --timeSlot )
		{
			if( timeToInputState > timeSlot->first )
			{
				break;
			}

			if( timeSlot == m_timeline.begin() )
			{
				outOfBounds = true;
			}
		}

		if( !outOfBounds )
		{
			//at this point we know that the time of the state to be inserted predate every other state in
			//the timeline
			m_timeline.push_front( std::pair< double, TimelineState >( timeToInputState, stateToInsert ) );
		}
		else
		{
			//timeSlot points to the element that should predate this
			++timeSlot;
			m_currentTimeSlot = m_timeline.insert( timeSlot, std::pair< double, TimelineState >( timeToInputState, stateToInsert ) );
		}
	}

	TimelineState Timeline::getState( double timeInSeconds ) const
	{
		TimelineState bestState;

		//see if the state is forward or backward in time from the current state
		if( m_timeline.empty() )
		{
			return TimelineState();
		}
		else
		{
			if( m_currentTimeSlot->first < timeInSeconds )
			{
				//we need to look forward
				bestState = getBestStateForwardInTime( timeInSeconds );
			}
			else if ( m_currentTimeSlot->first > timeInSeconds )
			{
				bestState = getBestStateBackwardInTime( timeInSeconds );
			}
			else
			{
				bestState = m_currentTimeSlot->second;
			}
		}

		return bestState;
	}

	TimelineState Timeline::getBestStateForwardInTime( double desiredStateTime ) const
	{
		std::list< std::pair< double, TimelineState > >::iterator currentState;
		for( currentState = m_currentTimeSlot; currentState != m_timeline.end(); ++currentState )
		{
			if( currentState->first > desiredStateTime || currentState->first == desiredStateTime )
			{
				break;
			}
		}

		if( currentState == m_timeline.end() )
		{
			//we need to extrapolate at this point
			--currentState;
			return extrapolateState( currentState, desiredStateTime );
		}
		else
		{
			if( currentState->first == desiredStateTime )
			{
				return currentState->second;
			}
			else
			{
				//we need to interpolate between the two values this state falls into
				std::list< std::pair< double, TimelineState > >::iterator initialState = currentState;
				initialState--;

				return interpolateState( initialState, currentState, desiredStateTime );
			}
		}
	}

	TimelineState Timeline::getBestStateBackwardInTime( double desiredStateTime ) const
	{
		bool outOfBounds = false;
		std::list< std::pair< double, TimelineState > >::iterator currentState;

		if( m_timeline.size() < 2 )
		{
			outOfBounds = true;
		}
		else
		{
			for( currentState = m_currentTimeSlot; !outOfBounds; --currentState )
			{
				if( currentState->first < desiredStateTime || currentState->first == desiredStateTime )
				{
					break;
				}

				if( currentState == m_timeline.begin() )
				{
					outOfBounds = true;
				}
			}
		}

		//if we are out of bounds, then there was no valid input state for the time we desired
		//for the moment, we return the beginning element on this case
		if( outOfBounds )
		{
			return currentState->second;
		}
		else
		{
			if( currentState->first == desiredStateTime )
			{
				return currentState->second;
			}
			else
			{
				std::list< std::pair< double, TimelineState > >::iterator nextState = currentState;
				++nextState;

				return interpolateState( currentState, nextState, desiredStateTime );
			}
		}
	}

	gh::TimelineState Timeline::extrapolateState( std::list< std::pair< double, TimelineState > >::iterator currentState,
													double desiredStateTime ) const
	{
		TimelineState extrapolatedState = currentState->second;
		double deltaTime = desiredStateTime - currentState->first;

		extrapolatedState.m_orientation += extrapolatedState.m_rotationalVelocity * deltaTime;

		Vector2 initialVelocity = extrapolatedState.m_velocity;

		extrapolatedState.m_velocity = initialVelocity + extrapolatedState.m_acceleration * float( deltaTime );
		extrapolatedState.m_velocity += initialVelocity * DRAG_COEFFICIENT;

		Vector2 distanceTraveled = extrapolatedState.m_velocity;
		distanceTraveled *= float( deltaTime );

		extrapolatedState.m_location += distanceTraveled;

		extrapolatedState.m_readyToFire = currentState->second.m_readyToFire;

		extrapolatedState.m_timeOfState = currentState->first;

		return extrapolatedState;
	}

	Vector2 Timeline::getLocalAcceleration( const Vector2& acceleration, float orientation ) const
	{
		const float radians = orientation * DEGREES_TO_RADIANS;

		Vector2 localAcceleration;
		localAcceleration.x = std::cos(radians) * acceleration.x - std::sin(radians) * acceleration.y;
		localAcceleration.y = std::sin(radians) * acceleration.x + std::cos(radians) * acceleration.y;

		return localAcceleration;
	}

	gh::TimelineState Timeline::interpolateState( std::list< std::pair< double, TimelineState > >::iterator initialState,
													std::list< std::pair< double, TimelineState > >::iterator currentState,
													double desiredStateTime ) const
	{
		float fractionComplete = RangeMapFloat( float( desiredStateTime ), float( initialState->first ), float( currentState->first ),
												0.f, 1.f, true );

		TimelineState interpolatedState;

		interpolatedState.m_acceleration = initialState->second.m_acceleration;

		float velocityX = InterpolateFloat( initialState->second.m_velocity.x, currentState->second.m_velocity.x, fractionComplete );
		float velocityY = InterpolateFloat( initialState->second.m_velocity.y, currentState->second.m_velocity.y, fractionComplete );

		interpolatedState.m_velocity = Vector2( velocityX, velocityY );

		interpolatedState.m_orientation = InterpolateFloat( initialState->second.m_orientation, currentState->second.m_orientation, fractionComplete );

		float locationX = InterpolateFloat( initialState->second.m_location.x, currentState->second.m_location.x, fractionComplete );
		float locationY = InterpolateFloat( initialState->second.m_location.y, currentState->second.m_location.y, fractionComplete );

		interpolatedState.m_location = Vector2( locationX, locationY );

		interpolatedState.m_rotationalVelocity = initialState->second.m_rotationalVelocity;

		interpolatedState.m_readyToFire = initialState->second.m_readyToFire;

		interpolatedState.m_timeOfState = initialState->first;

		return interpolatedState;
	}

}