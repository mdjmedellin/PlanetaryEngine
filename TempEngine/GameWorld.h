#ifndef GAME_WORLD_H
#define GAME_WORLD_H
#pragma once

//////////////////////////////////////////////////////////////////////////
///////////////////////////////////INCLUDES///////////////////////////////
#include <vector>
#include "Vector2.hpp"
#include <map>
#include "SpaceGameUtilities.hpp"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
namespace gh
{
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////FORWARD DECLARATIONS////////////////////
	class ActorData;
	class Actor;
	class NetworkingObject;
	class Bullet;
	class GameSimulator;
	class Message;

	namespace SpaceCombat
	{
		class Player;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////GAME WORLD////////////////////////////
	class GameWorld
	{
	public:
		GameWorld( const Vector2& WorldMaxCoords, GameSimulator* gameSimulator );
		~GameWorld();

		virtual void updateAllActors( double deltaTime );
		void killActor( unsigned int deadActorID );
		void sendActorInfo( NetworkingObject* networkConnection, std::vector< Message* >& messageQueue );
		void queueUpWorldActorsAddCommands( std::vector< Message* >& messageQueue );

		virtual void draw();
		void eraseAllActors();
		void addActor( Actor* newActor );

		virtual void addPlayer( SpaceCombat::Player* newPlayer );
		virtual void addBullet( Bullet* bullet );
		virtual void updateSimulation( double deltaTime );
		virtual void extractDeadActors();
		virtual void updateActor( const ActorData& actorInformation );

		void teleportToASafeLocation( SpaceCombat::Player* newPlayer );
		void insertState( double timeOffset, const TimelineState& stateToInsert );

	protected:
		void deleteDeadActors();
		void addActor( const ActorData& actorInfo );

		Vector2 m_worldMaxCoords;
		std::vector< Actor* > m_actors;
		std::vector< Actor* > m_actorsToRemoveNextFrame;
		std::map< unsigned int, Actor* > m_actorMap;
		GameSimulator* m_gameSimulator;
		ActorData* m_actorData;
	};
	//////////////////////////////////////////////////////////////////////////
}

#endif