#include "Actor.h"
#include "IncludeGL.h"
#include "GameWorld.h"
#include "AddActorCommand.hpp"

namespace gh
{
	Actor::Actor()
		:	m_location( Vector2() )
		,	m_mass( 0.f )
		,	scale( 1.f )
		,	m_radius( 0.f )
		,	m_orientation( 90.f )		// in degrees
		,	m_teams(0)
		,	m_gameWorld( nullptr )
		,	m_isAliveAndWell( true )
		,	m_id( s_nextID++ )
		,	m_actorType( NUMBER_OF_ACTOR_TYPES )
		,	m_addActorCommand( CMD_NONE )
		,	m_absoluteVelocity( 0.f )
	{}

	Actor::Actor( const ActorData& actorInformation, GameWorld* world )
		:	m_gameWorld( world )
		,	m_location( Vector2() )
		,	m_mass( 0.f )
		,	scale( 1.f )
		,	m_radius( 0.f )
		,	m_orientation( 90.f )		// in degrees
		,	m_teams(0)
		,	m_isAliveAndWell( true )
		,	m_id( s_nextID++ )
		,	m_actorType( NUMBER_OF_ACTOR_TYPES )
		,	m_addActorCommand( CMD_NONE )
		,	m_absoluteVelocity( 0.f )
	{
		setFromActorData( actorInformation );
	}

	Actor::~Actor()
	{}

	void Actor::setGameWorld( GameWorld* world )
	{
		m_gameWorld = world;
	}

	void Actor::draw()
	{
		glLoadIdentity();
		glTranslatef( m_location.x, m_location.y, 0 );
		glRotatef( m_orientation, 0, 0, 1 );

		glScalef( 1.f, 1.f, 1.f );
		glLineWidth( 2.f );


		glBegin( GL_LINE_LOOP );

		glVertex2f(	 0, 20 );
		glVertex2f(	 12, -10 );
		glVertex2f(	 6, -4 );
		glVertex2f( -6, -4 );
		glVertex2f( -12, -10 );

		glEnd();
	}

	float Actor::GetOrientation() const
	{
		return m_orientation;
	}

	void Actor::updateActor( const ActorData& actorInformation )
	{
		actorInformation.getLocation( m_location );
		m_orientation = actorInformation.m_orientation;
		m_isAliveAndWell = actorInformation.m_isAliveAndWell;
		m_teams = actorInformation.m_teams;
		m_velocity.x = actorInformation.m_velocityX;
		m_velocity.y = actorInformation.m_velocityY;
	}

	Vector2 Actor::getLocalAcceleration( const Vector2& acceleration )
	{
		const float radians = m_orientation * DEGREES_TO_RADIANS;

		Vector2 localAcceleration;
		localAcceleration.x = std::cos(radians) * acceleration.x - std::sin(radians) * acceleration.y;
		localAcceleration.y = std::sin(radians) * acceleration.x + std::cos(radians) * acceleration.y;

		return localAcceleration;
	}

	void Actor::applyLocalImpulse( float localImpulseX, float localImpulseY )
	{
		const float radians = m_orientation * DEGREES_TO_RADIANS;

		float worldImpulseX = std::cos(radians) * localImpulseX - std::sin(radians) * localImpulseY;
		float worldImpulseY = std::sin(radians) * localImpulseX + std::cos(radians) * localImpulseY;

		applyImpulse( worldImpulseX, worldImpulseY );
	}

	void Actor::applyImpulse( float worldImpulseX, float worldImpulseY )
	{
		if ( m_mass > 0 )
		{
			m_velocity.x += worldImpulseX / m_mass;
			m_velocity.y += worldImpulseY / m_mass;

			clampVelocity();
		}
	}

	void Actor::teleport( float newX, float newY )
	{
		m_location.SetXY( newX, newY );
	}

	bool Actor::isColliding( const Actor& otherActor) const
	{
		Vector2 delta = otherActor.m_location - m_location;

		const float distanceSquared = delta.CalculateLengthSquared();
		const float totalRadiusSquared = ( m_radius + otherActor.m_radius ) * ( m_radius + otherActor.m_radius );

		return distanceSquared < totalRadiusSquared;
	}

	void Actor::updateSimulation( double deltaTime, const Vector2& minCoords, const Vector2& maxCoords )
	{
		Vector2 distanceTraveled = m_velocity;
		distanceTraveled *= float( deltaTime );
		m_location += distanceTraveled;

		m_location.x = wrapComplex( m_location.x, minCoords.x, maxCoords.x );
		m_location.y = wrapComplex( m_location.y, minCoords.y, maxCoords.y );
	}

	bool Actor::isReadyToDie() const
	{
		return !m_isAliveAndWell;
	}

	void Actor::getActorData( ActorData* out_actorData ) const
	{
		out_actorData->m_id = m_id;
		out_actorData->setLocation( m_location );
		out_actorData->m_isAliveAndWell = m_isAliveAndWell;
		out_actorData->m_orientation = m_orientation;
		out_actorData->m_teams = m_teams;
		out_actorData->m_actorType = m_actorType;
		out_actorData->m_velocityX = m_velocity.x;
		out_actorData->m_velocityY = m_velocity.y;
	}

	void Actor::kill()
	{
		m_isAliveAndWell = false;
	}

	bool Actor::couldCollide( const Actor& otherActor ) const
	{
		if( m_isAliveAndWell )
		{
			return !( m_teams == otherActor.m_teams );
		}
		else
		{
			return false;
		}
	}

	bool Actor::diedBeforeTime() const
	{
		return false;
	}

	void Actor::setFromActorData( const ActorData& actorInformation )
	{
		updateActor( actorInformation );

		m_id = actorInformation.m_id;
		m_actorType = actorInformation.m_actorType;
	}

	void Actor::insertIntoTimeline( double timeOffset, const TimelineState& inputStateToInsert )
	{}

	void Actor::clampVelocity()
	{}

	Message* Actor::getAddActorMessage()
	{
		ActorData localActorData;
		this->getActorData( &localActorData );
		return new AddActorCommand( m_addActorCommand, localActorData );
	}

}