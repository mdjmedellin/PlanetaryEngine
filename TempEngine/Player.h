#ifndef PLAYER_H
#define PLAYER_H
#pragma once


//////////////////////////////////////////////////////////////////////////
//////////////////////////////INCLUDES////////////////////////////////////
#include "Actor.h"
#include "IncludeNetworkingLibraries.h"
#include "Timeline.hpp"
//////////////////////////////////////////////////////////////////////////


namespace gh
{

	//////////////////////////////////////////////////////////////////////////
	/////////////////////FORWARD DECLARATIONS/////////////////////////////////
	class KeyPadInputLogger;
	class Client;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////SPACE COMBAT////////////////////////////////////
	namespace SpaceCombat
	{
		class Player: public Actor
		{
		public:
			Player();
			Player( float playerXCord, float playerYCord );

			bool updateInput( KeyPadInputLogger* inputInformation );
			void getShipTipCoordinates( float& xHolder, float& yHolder, float padding ) const;
			void setOwnerInfo( unsigned long clientAddress, unsigned short clientPortNumber,
								const sockaddr_in& clientSockAddr, Client* clientOwner );
			void detachFromClient();
			void teleportToASafeLocation();
			void setFromTimelineState( const TimelineState& state );
			InputState getCurrentInputState() const;
			TimelineState getCurrentState() const;
			TimelineState getTimelineState( double timeInSeconds ) const;
			
			virtual bool diedBeforeTime() const;
			virtual void draw();
			virtual bool isReadyToDie() const;
			virtual void getActorData( ActorData* out_actorData ) const;
			virtual void kill();
			virtual void updateSimulation( double deltaTime, const Vector2& minCoords, const Vector2& maxCoords );
			virtual void insertIntoTimeline( double timeOffset, const TimelineState& stateToInsert );
			virtual Message* getAddActorMessage();

			unsigned long m_clientAddress;
			unsigned short m_clientPortNumber;
			bool m_clientIsAlive, m_isLocallyOwned;
			sockaddr_in m_clientSockAddr;

		private:
			void updateDynamics( const InputState& currentInputState, double deltaTime );
			void initializeDataMembers();
			void fireBullet();

			virtual void clampVelocity();

			bool m_readyToFireBullet, m_fireKeyPreviouslyDown;
			Client* m_owner;
			InputState m_currentInputState, m_prevInputState;
			Timeline m_Timeline;
		};
	}
	//////////////////////////////////////////////////////////////////////////
}

#endif