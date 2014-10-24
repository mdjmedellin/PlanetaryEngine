#include "Bullet.h"
#include "TimeUtility.h"
#include "IncludeGL.h"
#include "ActorData.h"

namespace gh
{
	Bullet::Bullet( unsigned int teamIndex )
		:	Actor()
		,	timeOfCreation( GetAbsoluteTimeSeconds() )
	{
		m_radius = 2.5f;
		m_teams = teamIndex;
		m_actorType = ACTOR_TYPE_BULLET;
		m_addActorCommand = CMD_ADD_BULLET;
	}

	Bullet::Bullet( ActorData* actorInformation )
		:	Actor()
	{
		m_id = actorInformation->m_id;
		actorInformation->getLocation( m_location );
		m_velocity = Vector2( actorInformation->m_velocityX, actorInformation->m_velocityY );
		m_orientation = actorInformation->m_orientation;
		m_teams = actorInformation->m_teams;
		m_radius = 2.5f;
		timeOfCreation = GetAbsoluteTimeSeconds() - actorInformation->m_timeAlive;
		m_actorType = ACTOR_TYPE_BULLET;
		m_addActorCommand = CMD_ADD_BULLET;
	}

	Bullet::~Bullet()
	{}

	void Bullet::draw()
	{
		glLoadIdentity();
		glTranslatef( m_location.x, m_location.y, 0 );
		glRotatef( m_orientation, 0, 0, 1 );
		glLineWidth(2.5); 
		glBegin(GL_LINES);
		glVertex2f(0.f, 0.f);
		glVertex2f(5.f, 0.f);
		glEnd();
	}

	void Bullet::setVelocity( float shipOrientation )
	{
		m_orientation = shipOrientation;

		m_velocity.x = cos( (m_orientation) * DEGREES_TO_RADIANS ) * 100.f;
		m_velocity.y = sin( (m_orientation) * DEGREES_TO_RADIANS ) * 100.f;
	}

	bool Bullet::isReadyToDie() const
	{
		if( !m_isAliveAndWell )
		{
			return true;
		}
		else
		{
			return ( GetAbsoluteTimeSeconds() - timeOfCreation > 2.0 );
		}
	}

	bool Bullet::diedBeforeTime()
	{
		return !m_isAliveAndWell;
	}

	void Bullet::getActorData( ActorData* out_actorData )
	{
		Actor::getActorData( out_actorData );

		out_actorData->m_timeAlive = GetAbsoluteTimeSeconds() - timeOfCreation;
	}
}