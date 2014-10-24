#ifndef ACTOR_DATA_H
#define ACTOR_DATA_H
#pragma once

class Vector2;

namespace gh
{
	enum ActorType{	ACTOR_TYPE_PLAYER,
					ACTOR_TYPE_BULLET,
					NUMBER_OF_ACTOR_TYPES	};

	class ActorData
	{
	public:
		void getLocation( Vector2& out_Location ) const;
		void getAcceleration( Vector2& out_Acceleration ) const;
		void setAcceleration( const Vector2& acceleration );
		void setLocation( const Vector2& location );

		unsigned int m_id;
		float m_xCoord, m_yCoord;
		bool m_isAliveAndWell;
		float m_orientation;
		unsigned int m_teams;
		float m_velocityX, m_velocityY;
		double m_timeAlive;
		ActorType m_actorType;
		float m_accelerationX, m_accelerationY;
		float m_rotationalVelocity;
	};
}


#endif