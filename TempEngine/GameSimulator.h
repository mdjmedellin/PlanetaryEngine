#ifndef GAME_SIMULATOR_H
#define GAME_SIMULATOR_H
#pragma once

//////////////////////////////////////////////////////////////////////////
////////////////////////////////////INCLUDES//////////////////////////////
#include <string>
#include "TimeUtility.h"
#include "IncludeNetworkingLibraries.h"
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
////////////////////////////FORWARD DECLARATIONS / ENUMS//////////////////
class Vector2;

enum ClientState{	CLIENT_LOGGED_OUT,
					CLIENT_LOGGED_IN	};

enum ServerState{	SERVER_RUNNING,
					SERVER_DOWN,
					SERVER_SHUTTING_DOWN };

enum GameStatus{	GOOD_STATUS,
					BAD_STATUS	};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
namespace gh
{

	//////////////////////////////////////////////////////////////////////////
	class KeyPadInputLogger;
	class NetworkingObject;
	class GameWorld;
	class ActorData;

	namespace SpaceCombat
	{
		class Player;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////GAME SIMULATOR/////////////////////
	class GameSimulator
	{
	public:
		GameSimulator();
		~GameSimulator();

		virtual void initializeGameWorld( const Vector2& windowDimensions ) = 0;
		virtual void initializeNetworkConnection( unsigned short serverPortNumber = 0, const std::string& serverIPAddress = "" ) = 0;
		virtual void sendInput() = 0;
		virtual void sendUpdate() = 0;
		virtual void receivePackets() = 0;
		virtual void updateState() = 0;
		virtual void onResize( const Vector2& WindowDimensions ) = 0;
		virtual void renderGame() = 0;
		virtual void updateSimulation() = 0;
		
		void updateGame();
		void updateInput();
		
		void onKeyDown( const char& key );
		void onKeyUp( const char& key );

		bool isInGoodStatus();

	protected:
		void sendRTTResponse( unsigned int rttID, const sockaddr_in& receivingAddress );

		//member variables
		KeyPadInputLogger* m_inputLogger;
		NetworkingObject* m_connection;
		GameWorld* m_gameWorld;
		double m_timeOfLastSimulation, m_timeOfLastSentPacket, m_timeOfLastReceivedPacket;
		gh::SystemClocks m_nextSimulationTimeClocks;
		ActorData* m_actorData;
		GameStatus m_gameStatus;
	};
}

#endif