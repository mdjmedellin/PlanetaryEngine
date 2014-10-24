#include "GameWorld.h"
#include "SocketManager.h"
#include "NetworkingObject.h"
#include "Vector2.hpp"
#include "TimeUtility.h"
#include "Actor.h"
#include "ActorData.h"
#include "Player.h"
#include "MathUtilities.hpp"
#include "Bullet.h"
#include "GameSimulator.h"
#include "UpdateActorCommand.hpp"
#include "AddActorCommand.hpp"
#include <assert.h>

namespace gh
{
	GameWorld::GameWorld( const Vector2& WorldMaxCoords, GameSimulator* gameSimulator )
		:	m_worldMaxCoords( WorldMaxCoords )
		,	m_gameSimulator( gameSimulator )
		,	m_actorData( new ActorData() )
	{}

	GameWorld::~GameWorld()
	{
		eraseAllActors();
		delete m_actorData;
	}

	void GameWorld::eraseAllActors()
	{
		for( std::vector< Actor* >::iterator actorToBeDeleted = m_actors.begin(); actorToBeDeleted != m_actors.end(); )
		{
			if( ( *actorToBeDeleted ) != nullptr )
			{
				delete ( *actorToBeDeleted );
			}

			actorToBeDeleted = m_actors.erase( actorToBeDeleted );
		}
	}

	void GameWorld::draw()
	{
		for( std::vector< Actor* >::iterator currentActor = m_actors.begin(); currentActor != m_actors.end(); ++currentActor )
		{
			if( ( *currentActor ) != nullptr )
			{
				( *currentActor )->draw();
			}
		}
	}

	void GameWorld::updateActor( const ActorData& actorInformation )
	{
		//find the actor in the map
		std::map< unsigned int, Actor* >::iterator searchResult = m_actorMap.find( actorInformation.m_id );

		if( searchResult != m_actorMap.end() )
		{
			searchResult->second->updateActor( actorInformation );
		}
	}

	void GameWorld::killActor( unsigned int deadActorID )
	{
		//look for the actor in the map
		std::map< unsigned int, Actor* >::iterator searchResult = m_actorMap.find( deadActorID );

		if( searchResult != m_actorMap.end() )
		{
			//we found the actor
			searchResult->second->kill();

			//remove the entry from the map
			m_actorMap.erase( searchResult );
		}
	}

	void GameWorld::addActor( const ActorData& actorInfo )
	{
		m_actors.push_back( new Actor( actorInfo, this ) );
	}

	void GameWorld::addActor( Actor* newActor )
	{
		std::map< unsigned int, Actor* >::iterator searchResult = m_actorMap.find( newActor->m_id );

		//this ensures that we only have one actor with the same id 
		if( searchResult == m_actorMap.end() )
		{
			m_actorMap[ newActor->m_id ] = newActor;
			m_actors.push_back( newActor );
		}

		//this should return a notification of wether it was added or not
	}

	void GameWorld::addPlayer( SpaceCombat::Player* newPlayer )
	{
		//teleport the player to a safe location
		teleportToASafeLocation( newPlayer );
		addActor( newPlayer );
	}

	void GameWorld::teleportToASafeLocation( SpaceCombat::Player* newPlayer )
	{
		bool spaceIsSafe = false;
		float playerXCord, playerYCord;

		newPlayer->setGameWorld( this );

		while(!spaceIsSafe)
		{
			spaceIsSafe = true;

			playerXCord = InterpolateFloat( 0, m_worldMaxCoords.x, RandZeroToOne() );
			playerYCord = InterpolateFloat( 0, m_worldMaxCoords.y, RandZeroToOne() );
			newPlayer->teleport( playerXCord, playerYCord );

			for( std::vector< Actor* >::iterator currentActor = m_actors.begin();
				currentActor != m_actors.end(); ++currentActor )
			{
				if ( ( ( *currentActor )->couldCollide( *newPlayer ) || newPlayer->couldCollide( **currentActor ) )
					&& ( *currentActor )->isColliding( *newPlayer ) )
				{
					spaceIsSafe = false;
					break;
				}
			}
		}
	}

	void GameWorld::updateSimulation( double deltaTime )
	{}

	void GameWorld::addBullet( Bullet* bullet )
	{}

	void GameWorld::queueUpWorldActorsAddCommands( std::vector< Message* >& messageQueue )
	{
		for( std::vector< Actor* >::iterator currentActor = m_actors.begin();
			currentActor != m_actors.end(); ++currentActor )
		{
			messageQueue.push_back( ( *currentActor )->getAddActorMessage() );
		}
	}

	void GameWorld::sendActorInfo( NetworkingObject* networkConnection, std::vector< Message* >& messageQueue )
	{
		for( std::vector< Actor* >::iterator currentActor = m_actors.begin();
			currentActor != m_actors.end(); ++currentActor )
		{
			( *currentActor )->getActorData( m_actorData );
			messageQueue.push_back( new UpdateActorCommand( CMD_UPDATE_ACTOR, *m_actorData ) );
		}
	}

	void GameWorld::updateAllActors( double deltaTime )
	{
		for( std::vector< Actor* >::iterator currentActor = m_actors.begin(); currentActor != m_actors.end(); ++currentActor )
		{
			if( ( *currentActor ) && !( *currentActor )->isReadyToDie() )
			{
				( *currentActor )->updateSimulation( deltaTime, Vector2(), m_worldMaxCoords );
			}
		}
	}

	void GameWorld::deleteDeadActors()
	{
		for( std::vector< Actor* >::iterator currentDeadActor = m_actorsToRemoveNextFrame.begin();
			currentDeadActor != m_actorsToRemoveNextFrame.end(); )
		{
			delete (*currentDeadActor);
			currentDeadActor = m_actorsToRemoveNextFrame.erase( currentDeadActor );
		}
	}

	void GameWorld::extractDeadActors()
	{}

	void GameWorld::insertState( double timeOffset, const TimelineState& stateToInsert )
	{
		for( std::vector< Actor* >::iterator currentActor = m_actors.begin();
			currentActor != m_actors.end(); ++currentActor )
		{
			if( ( *currentActor )->m_id == stateToInsert.m_actorID )
			{
				( *currentActor )->insertIntoTimeline( timeOffset, stateToInsert );
			}
		}
	}

}
