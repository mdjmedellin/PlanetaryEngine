#include "GameSimulator.h"
#include "KeyPadInputLogger.h"
#include "SocketManager.h"
#include "GameWorld.h"
#include "NetworkingObject.h"
#include "ActorData.h"
#include "Player.h"
#include "GeneralCommand.hpp"

namespace gh
{
	GameSimulator::GameSimulator()
		:	m_timeOfLastSimulation( GetAbsoluteTimeSeconds() - 20.0 )
		,	m_inputLogger( new KeyPadInputLogger() )
		,	m_actorData( new ActorData() )
		,	m_gameStatus( GOOD_STATUS )
		,	m_timeOfLastSentPacket( GetAbsoluteTimeSeconds() - 20.0 )
		,	m_timeOfLastReceivedPacket( GetAbsoluteTimeSeconds() + 20.0 )
		,	m_nextSimulationTimeClocks( 0.0 )
	{
		//initialize networking
		if( initializeNetworking( MAKEWORD( 2, 2 ) ) != NO_ERROR )
		{
			printf( "ERROR: Could not start WinSock DLL\n" );
			m_gameStatus = BAD_STATUS;
		}
		else
		{
			printf( "NETWORKING STARTED\n" );
		}	
	}

	void GameSimulator::initializeGameWorld( const Vector2& worldDimensions )
	{
		m_gameWorld = new GameWorld( worldDimensions, this );
	}

	void GameSimulator::onKeyDown( const char& key )
	{
		m_inputLogger->onKeyDown( key );
	}

	void GameSimulator::onKeyUp( const char& key )
	{
		m_inputLogger->onKeyUp( key );
	}

	void GameSimulator::onResize( const Vector2& WindowDimensions )
	{}

	GameSimulator::~GameSimulator()
	{
		if( m_inputLogger )
		{
			delete m_inputLogger;
		}

		SocketManager::destroy();

		endNetworking();

		//kill the game world
		if( m_gameWorld )
		{
			delete m_gameWorld;
			m_gameWorld = nullptr;
		}
	}

	void GameSimulator::sendInput()
	{}

	void GameSimulator::receivePackets()
	{}

	void GameSimulator::renderGame()
	{}

	void GameSimulator::updateState()
	{}

	void GameSimulator::sendUpdate()
	{}

	void GameSimulator::updateGame()
	{
		//receive all the packets that you have
		receivePackets();

		//update the game simulation
		updateSimulation();

		//send the actor's position
		sendUpdate();

		updateState();
	}

	void GameSimulator::updateSimulation()
	{}

	bool GameSimulator::isInGoodStatus()
	{
		return m_gameStatus == GOOD_STATUS;
	}

	void GameSimulator::sendRTTResponse( unsigned int rttID, const sockaddr_in& receivingAddress )
	{
		GeneralCommand cmd( CMD_RTT_ACK, rttID );
		cmd.serialize( m_connection->m_messageBuffer );
		m_connection->sendPacketTo( receivingAddress );
	}

}