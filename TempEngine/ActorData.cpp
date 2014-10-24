#include "ActorData.h"
#include "Vector2.hpp"

namespace gh
{
	void ActorData::getLocation( Vector2& out_Location ) const
	{
		out_Location.SetXY( m_xCoord, m_yCoord );
	}

	void ActorData::setLocation( const Vector2& location )
	{
		m_xCoord = location.x;
		m_yCoord = location.y;
	}

	void ActorData::getAcceleration( Vector2& out_Acceleration ) const
	{
		out_Acceleration.SetXY( m_accelerationX, m_accelerationY );
	}

	void ActorData::setAcceleration( const Vector2& acceleration )
	{
		m_accelerationX = acceleration.x;
		m_accelerationY = acceleration.y;
	}

}