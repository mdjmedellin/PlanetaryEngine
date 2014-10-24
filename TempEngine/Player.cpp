#include "Player.h"
#include "IncludeGL.h"
#include "KeyPadInputLogger.h"
#include "Bullet.h"
#include "GameWorld.h"
#include "Client.hpp"
#include "TimeUtility.h"
#include "AddTimelineActorCommand.hpp"

namespace
{
	const float MAX_ABSOLUTE_VELOCITY = 300.f;
	const float DRAG_COEFFICIENT = .25f;
	const double SHOOTING_BUFFER_TIME = 0.100;
}

namespace gh
{
	namespace SpaceCombat
	{
		Player::Player()
			:	Actor()
			,	m_owner( nullptr )
			,	m_currentInputState( InputState() )
			,	m_prevInputState( InputState() )
			,	m_isLocallyOwned( false )
			,	m_fireKeyPreviouslyDown( false )
		{
			initializeDataMembers();
		}

		Player::Player( float playerXCord, float playerYCord )
			:	Actor()
			,	m_owner( nullptr )
			,	m_currentInputState( InputState() )
			,	m_prevInputState( InputState() )
			,	m_isLocallyOwned( false )
			,	m_fireKeyPreviouslyDown( false )
		{
			initializeDataMembers();

			m_location.SetXY( playerXCord, playerYCord );
		}

		void Player::initializeDataMembers()
		{
			m_clientAddress = 0;
			m_clientPortNumber = 0;
			m_clientIsAlive = true;
			m_radius = 20.f;
			m_mass = 1.f;
			m_teams = s_nextTeam++;
			m_readyToFireBullet = false;
			m_actorType = ACTOR_TYPE_PLAYER;
			m_addActorCommand = CMD_ADD_SHIP;

			m_Timeline.insertState( 0.0, TimelineState() );
		}

		bool Player::updateInput( KeyPadInputLogger* inputInformation )
		{
			static const float thrustAmount = 100.0f;
			static const float rotationAmount = 200.0f;

			m_currentInputState.reset();
			m_currentInputState.m_actorID = m_id;

			if( inputInformation->isKeyPressed( 'w' ) || inputInformation->isKeyPressed( 'W' ) ) m_currentInputState.m_acceleration.y += thrustAmount;
			if( inputInformation->isKeyPressed( 'a' ) || inputInformation->isKeyPressed( 'A' ) ) m_currentInputState.m_rotationChange += rotationAmount;
			if( inputInformation->isKeyPressed( 'd' ) || inputInformation->isKeyPressed( 'D' ) ) m_currentInputState.m_rotationChange -= rotationAmount;
			if( inputInformation->isKeyPressed( ' ' ) ) m_fireKeyPreviouslyDown = true;
			if( m_fireKeyPreviouslyDown && !inputInformation->isKeyPressed( ' ' ) )
			{
					m_currentInputState.m_readyToFire = true;
					m_fireKeyPreviouslyDown = false;
			}

			m_currentInputState.m_acceleration = getLocalAcceleration( m_currentInputState.m_acceleration );

			bool stateChanged = false;

			//if( m_currentInputState != m_prevInputState || m_currentInputState.readyToFireBullet )
			if( m_currentInputState != m_prevInputState )
			{
				stateChanged = true;
			}

			m_prevInputState = m_currentInputState;

			return stateChanged;
		}

		void Player::draw()
		{
			//Draw the Player Spaceship
			//
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

		void Player::getShipTipCoordinates( float& xHolder, float& yHolder, float padding ) const
		{
			float distanceFromCenter = m_radius + padding;
			float angleRadians = ConvertDegreesToRadians( m_orientation + 90 );
			xHolder = distanceFromCenter * cos( angleRadians ) + m_location.x;
			yHolder = distanceFromCenter * sin( angleRadians ) + m_location.y;
		}

		void Player::setOwnerInfo( unsigned long clientAddress, unsigned short clientPortNumber, 
								const sockaddr_in& clientSockAddr, Client* clientOwner )
		{
			m_clientAddress = clientAddress;
			m_clientPortNumber = clientPortNumber;
			m_clientSockAddr = clientSockAddr;
			m_owner = clientOwner;
		}

		void Player::detachFromClient()
		{
			m_clientAddress = 0;
			m_clientPortNumber = 0;
			m_owner = nullptr;
			m_clientIsAlive = false;
		}

		bool Player::isReadyToDie() const
		{
			if( m_clientIsAlive )
			{
				//check if it is already marked to die
				return !m_isAliveAndWell;
			}
			else
			{
				return true;
			}
		}

		void Player::fireBullet()
		{
			Bullet* newBullet = new Bullet( m_teams );

			float bulletXCord, bulletYCord;
			getShipTipCoordinates( bulletXCord, bulletYCord, 5.f);
			newBullet->teleport( bulletXCord, bulletYCord );
			newBullet->setVelocity( m_orientation + 90.f );
			m_gameWorld->addBullet( newBullet );

			delete newBullet;
		}

		void Player::getActorData( ActorData* out_actorData ) const
		{
			Actor::getActorData( out_actorData );
			out_actorData->setAcceleration( m_currentInputState.m_acceleration );
			out_actorData->m_rotationalVelocity = m_currentInputState.m_rotationChange;
			//out_actorData->m_readyToFireBullet = true;
		}

		bool Player::diedBeforeTime() const
		{
			return true;
		}

		void Player::kill()
		{
			if( m_owner != nullptr )
			{
				//tell the owner that its player is dying
				m_owner->killPlayer( true );
			}
			else
			{
				Actor::kill();
			}
		}

		void Player::teleportToASafeLocation()
		{
			m_gameWorld->teleportToASafeLocation( this );
			m_velocity.SetXY( 0.f, 0.f );
		}

		void Player::updateSimulation( double deltaTime, const Vector2& minCoords, const Vector2& maxCoords )
		{
			//queue the timeline for the input and update our acceleration, velocity and rotation according to it
			/*if( m_isLocallyOwned )
			{
				updateDynamics( m_currentInputState, deltaTime );
			}
			else*/

				TimelineState currentState = m_Timeline.getState( GetAbsoluteTimeSeconds() );
				m_velocity = currentState.m_velocity;
				m_orientation = currentState.m_orientation;
				m_location = currentState.m_location;
			
			m_location.x = wrapComplex( m_location.x, minCoords.x, maxCoords.x );
			m_location.y = wrapComplex( m_location.y, minCoords.y, maxCoords.y );

			if( currentState.m_readyToFire )
			{
				if( GetAbsoluteTimeSeconds() - currentState.m_timeOfState  < SHOOTING_BUFFER_TIME )
				{
					fireBullet();
				}
				double timeOfState = GetAbsoluteTimeSeconds();
				currentState.m_readyToFire = false;
				m_Timeline.insertState( timeOfState, currentState );
			}
		}

		void Player::updateDynamics( const InputState& currentInputState, double deltaTime )
		{
			m_orientation += currentInputState.m_rotationChange * deltaTime;
			Vector2 localAcceleration = getLocalAcceleration( currentInputState.m_acceleration );

			Vector2 distanceTraveled = m_velocity;
			distanceTraveled *= float( deltaTime );
			distanceTraveled +=  0.5f * ( localAcceleration - m_velocity * DRAG_COEFFICIENT ) * float( deltaTime ) * float( deltaTime );

			m_location += distanceTraveled;

			m_velocity += deltaTime * ( localAcceleration - m_velocity * DRAG_COEFFICIENT );
		}

		InputState Player::getCurrentInputState() const
		{
			return m_currentInputState;
		}

		void Player::insertIntoTimeline( double timeOffset, const TimelineState& stateToInsert )
		{
			m_Timeline.insertState( timeOffset, stateToInsert );
		}

		void Player::clampVelocity()
		{
			if( m_velocity.CalculateLengthSquared() > MAX_ABSOLUTE_VELOCITY * MAX_ABSOLUTE_VELOCITY )
			{
				m_velocity.SetLength( MAX_ABSOLUTE_VELOCITY );
			}
		}

		TimelineState Player::getCurrentState() const
		{
			ActorData tempActorData;
			getActorData( &tempActorData );

			TimelineState currentState( &tempActorData );

			return currentState;
		}

		Message* Player::getAddActorMessage()
		{
			double timeOfState = GetAbsoluteTimeSeconds();
			TimelineState stateOfShip = m_Timeline.getState( timeOfState );
			return new AddTimelineActorCommand( m_addActorCommand, timeOfState, stateOfShip );
		}

		TimelineState Player::getTimelineState( double timeInSeconds ) const
		{
			return m_Timeline.getState( timeInSeconds );
		}

		void Player::setFromTimelineState( const TimelineState& state )
		{
			m_location = state.m_location;
			m_velocity = state.m_velocity;
			m_orientation = state.m_orientation;
			m_id = state.m_actorID;
			m_teams = state.m_teams;
		}

	}
}