#ifndef SPACE_GAME_UTILITIES_H
#define SPACE_GAME_UTILITIES_H
#pragma once

#include "Vector2.hpp"
#include "ActorData.h"

namespace gh
{
	struct InputState
	{
		InputState()
			:	m_impulse( Vector2() )
			,	m_acceleration( Vector2() )
			,	m_rotationChange( 0.f )
			,	m_readyToFire( false )
			,	m_actorID( 0 )
		{}

		void reset()
		{
			m_impulse.SetXY( 0.f, 0.f );
			m_acceleration.SetXY( 0.f, 0.f );
			m_rotationChange = 0.f;
			m_readyToFire = false;
			m_actorID = 0;
		}

		bool operator==(const InputState &other) const
		{
			return m_impulse == other.m_impulse
					&&	m_rotationChange == other.m_rotationChange
					&&	m_readyToFire == other.m_readyToFire
					&&	m_acceleration == other.m_acceleration;
		}

		bool operator!=( const InputState& other ) const
		{
			return !( *this == other );
		}

		Vector2 m_impulse, m_acceleration;
		float m_rotationChange;
		float m_readyToFire;
		unsigned int m_actorID;
	};


	struct TimelineState
	{
		TimelineState()
			:	m_location( Vector2() )
			,	m_velocity( Vector2() )
			,	m_acceleration( Vector2() )
			,	m_orientation( 0.f )
			,	m_rotationalVelocity( 0.f )
			,	m_actorID( 0 )
			,	m_teams( 0 )
			,	m_readyToFire( false )
			,	m_timeOfState( 0.0 )
		{}

		explicit TimelineState( const ActorData* actorData )
			:	m_readyToFire( false )
		{
			actorData->getLocation( m_location );
			m_velocity.SetXY( actorData->m_velocityX, actorData->m_velocityY );
			actorData->getAcceleration( m_acceleration );
			m_orientation = actorData->m_orientation;
			m_rotationalVelocity = actorData->m_rotationalVelocity;
			m_actorID = actorData->m_id;
			m_teams = actorData->m_teams;
		}

		void setInputInformation( const InputState& currentInputState )
		{
			m_acceleration = currentInputState.m_acceleration;
			m_rotationalVelocity = currentInputState.m_rotationChange;
			m_readyToFire = currentInputState.m_readyToFire;
		}

		Vector2 m_location, m_velocity, m_acceleration;
		float m_orientation, m_rotationalVelocity;
		unsigned int m_actorID, m_teams;
		bool m_readyToFire;
		double m_timeOfState;
	};
}

#endif