#ifndef ACTOR_H
#define ACTOR_H
#pragma once

//////////////////////////////////////////////////////////////////////////
///////////////////////INCLUDES///////////////////////////////////////////
#include "Vector2.hpp"
#include "ActorData.h"
#include "PacketHeader.h"
#include "SpaceGameUtilities.hpp"
//////////////////////////////////////////////////////////////////////////

namespace gh
{
	//////////////////////////////////////////////////////////////////////////
	///////////////////////FORWARD DECLARATIONS///////////////////////////////
	class GameWorld;
	class Message;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	////////////////////////////ACTOR/////////////////////////////////////////
	class Actor
	{
	public:
		Actor( const ActorData& actorInformation, GameWorld* world );
		Actor();
		virtual ~Actor();

		void setFromActorData( const ActorData& actorInformation );
		void updateActor( const ActorData& actorInformation );
		void setGameWorld( GameWorld* world );
		void teleport( float xCoord, float yCoord );
		float GetOrientation( ) const;
		bool isColliding( const Actor& otherActor) const;

		virtual void insertIntoTimeline( double timeOffset, const TimelineState& stateToInsert );
		virtual void updateSimulation( double deltaTime, const Vector2& minCoords, const Vector2& maxCoords );
		virtual void draw();
		virtual bool isReadyToDie() const;
		virtual void getActorData( ActorData* out_actorData ) const;
		virtual bool couldCollide( const Actor& otherActor ) const;
		virtual bool diedBeforeTime() const;
		virtual void kill();
		virtual Message* getAddActorMessage();

		unsigned int m_id;
		unsigned int m_teams;
		PacketCommandType m_addActorCommand;

	protected:

		Vector2 getLocalAcceleration( const Vector2& acceleration );
		void applyLocalImpulse( float localImpulseX, float localImpulseY );
		void applyImpulse( float worldImpulseX, float worldImpulseY );

		virtual void clampVelocity();
		

		float m_mass;							// if mass == 0, the mass is infinite.
		Vector2 m_location;
		Vector2 m_velocity;
		float scale;
		float m_radius;
		float m_orientation;					// in degrees
		float m_absoluteVelocity;
		//unsigned int m_teams;					// used to identify which actors destroy other actors
		GameWorld* m_gameWorld;
		bool m_isAliveAndWell;
		ActorType m_actorType;
	};
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	/////////////////////STATIC VARIABLES/////////////////////////////////////
	static unsigned int s_nextID = 0;
	static unsigned int s_nextTeam = 0;
	//////////////////////////////////////////////////////////////////////////
}

#endif