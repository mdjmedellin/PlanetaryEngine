#include "Client.hpp"
#include "Player.h"
#include "Message.hpp"
#include "GeneralCommand.hpp"
#include "NetworkingObject.h"
#include "ActorData.h"
#include "UpdateActorCommand.hpp"

namespace
{
	const double RTT_SMOOTHING_FACTOR = 0.15;
}

namespace gh
{
	Client::Client()
		:	m_player( new SpaceCombat::Player() )
		,	m_currentPacketNumber( -1 )
		,	m_lastHeardFromTime( 0.0 )
		,	m_port( 0 )
		,	m_address( 0 )
		,	m_RTTCounter( 0 )
		,	m_RTTArrived( false )
		,	m_currentRTT( 0 )
		,	m_timeOfLastSentRTT( 0 )
		,	m_clientIsDropped( false )
		,	m_timeOfLastSentGuaranteedMessage( 0.0 )
		,	m_newGuaranteedMessage( false )
		,	m_isInitialized( false )
	{
		m_player->m_clientIsAlive = true;
	}

	Client::Client( unsigned short portNumber, unsigned long address, const sockaddr_in& socketAddress )
		:	m_player( new SpaceCombat::Player() )
		,	m_currentPacketNumber( -1 )
		,	m_lastHeardFromTime( 0.0 )
		,	m_port( portNumber )
		,	m_address( address )
		,	m_socketAddress( socketAddress )
		,	m_RTTCounter( 0 )
		,	m_RTTArrived( false )
		,	m_currentRTT( 0 )
		,	m_timeOfLastSentRTT( 0 )
		,	m_clientIsDropped( false )
		,	m_timeOfLastSentGuaranteedMessage( 0.0 )
		,	m_newGuaranteedMessage( false )
		,	m_isInitialized( false )
	{
		m_player->m_clientIsAlive = true;
		m_player->setOwnerInfo( m_address, m_port, m_socketAddress, this );
	}

	Client::~Client()
	{}

	void Client::updateLastHeardFromTime( double newTimeSeconds )
	{
		m_lastHeardFromTime = newTimeSeconds;
	}

	void Client::queueRTTMessage()
	{
		if( !m_RTTArrived )
		{
			m_currentRTT = GetAbsoluteTimeSeconds() - m_timeOfLastSentRTT;
		}

		m_RTTArrived = false;
		++m_RTTCounter;
		m_messageQueue.push_back( new GeneralCommand( CMD_RTT, m_RTTCounter ) );
		m_timeOfLastSentRTT = GetAbsoluteTimeSeconds();
	}

	void Client::processRTT( unsigned int rttMessageIndex )
	{
		if( m_RTTCounter == rttMessageIndex && !m_RTTArrived )
		{
			double newRTTSample = GetAbsoluteTimeSeconds() - m_timeOfLastSentRTT;

			m_currentRTT = ( RTT_SMOOTHING_FACTOR * m_currentRTT ) + ( 1.0 - RTT_SMOOTHING_FACTOR ) * newRTTSample;
			m_RTTArrived = true;
		}
	}

	void Client::sendMessage( NetworkingObject* connection )
	{
		m_lastMessageQueue = m_messageQueue;

		if( m_messageQueue.empty() )
		{
			GeneralCommand cmd( CMD_NONE );
			cmd.serialize( connection->m_messageBuffer );
		}
		else
		{
			for( std::vector< Message* >::iterator currentMessage = m_messageQueue.begin();
				currentMessage != m_messageQueue.end(); ++currentMessage )
			{
				( *currentMessage )->serialize( connection->m_messageBuffer );
			}
		}

		//after adding all the messages to the buffer, we send the message
		connection->sendPacketTo( m_socketAddress );
		m_messageQueue.clear();
	}

	void Client::addGuaranteedMessage( Message* guaranteedMessage )
	{
		guaranteedMessage->setAsGuaranteed();
		m_guaranteedMessageMap.insert( std::pair< unsigned int, Message* >( guaranteedMessage->getPacketID(), guaranteedMessage ) );
		guaranteedMessage->subscribers++;
		m_newGuaranteedMessage = true;
	}

	bool Client::hasProcessedAllGuaranteedMessages() const
	{
		return m_guaranteedMessageMap.empty();
	}

	void Client::sendGuaranteedMessages( NetworkingObject* m_connection )
	{
		for( std::map< unsigned int, Message* >::iterator guaranteedMessage = m_guaranteedMessageMap.begin();
			guaranteedMessage != m_guaranteedMessageMap.end(); ++guaranteedMessage )
		{
			guaranteedMessage->second->serialize( m_connection->m_messageBuffer );
		}

		//now that we have all the guaranteed messages in the queue. We send it to the client
		m_connection->sendPacketTo( m_socketAddress );

		m_newGuaranteedMessage = false;
	}

	void Client::acknowledgeMessage( unsigned int messageID )
	{
		std::map< unsigned int, Message* >::iterator guaranteedMessageSearchResult = m_guaranteedMessageMap.find( messageID );

		if( guaranteedMessageSearchResult != m_guaranteedMessageMap.end() )
		{
			guaranteedMessageSearchResult->second->acknowledgePacket();
			guaranteedMessageSearchResult->second->subscribers--;

			if( guaranteedMessageSearchResult->second->subscribers == 0 )
			{
				delete guaranteedMessageSearchResult->second;
			}

			m_guaranteedMessageMap.erase( guaranteedMessageSearchResult );
		}
	}

	bool Client::actorIsAlive() const
	{
		if( m_player != nullptr && !m_player->isReadyToDie() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void Client::updateActor( const ActorData* updatedActorData )
	{
		//this is a stale state
		TimelineState staleState( updatedActorData );

		m_player->updateActor( *updatedActorData );
	}

	void Client::killPlayer( bool respawnAfterKill/* = false */)
	{
		if( m_player == nullptr )
		{
			return;
		}
		else
		{
			m_player->detachFromClient();
			m_player = nullptr;
		}
	}

	void Client::setAsDropped()
	{
		m_clientIsDropped = true;
		killPlayer();
	}

	bool Client::isDropped() const
	{
		return m_clientIsDropped;
	}

	bool Client::hasPlayerID( unsigned long playerID )
	{
		if( m_player != nullptr )
		{
			return m_player->m_id == playerID;
		}
		else
		{
			return false;
		}
	}

	void Client::sendAcknowledgements( NetworkingObject* connection )
	{
		if( m_acknowledgementQueue.empty() )
		{
			return;
		}
		else
		{
			for( std::vector< Message* >::iterator currenctAckMessage = m_acknowledgementQueue.begin();
				currenctAckMessage != m_acknowledgementQueue.end(); )
			{
				( *currenctAckMessage )->serialize( connection->m_messageBuffer );

				//delete the message and remove it form the vector
				delete ( *currenctAckMessage );
				currenctAckMessage = m_acknowledgementQueue.erase( currenctAckMessage );
			}

			m_acknowledgementQueue.clear();
		}

		connection->sendPacketTo( m_socketAddress );
	}

	bool Client::respawnPlayer()
	{
		if( m_player != nullptr )
		{
			return false;
		}
		else
		{
			m_player = new SpaceCombat::Player();
			m_player->m_clientIsAlive = true;
			m_player->setOwnerInfo( m_address, m_port, m_socketAddress, this );
			return true;
		}
	}

	double Client::getTimeOfLastSentGuaranteedMessage() const
	{
		return m_timeOfLastSentGuaranteedMessage;
	}

	void Client::updateTimeOfLastGuaranteedMessage( double newTimeSeconds )
	{
		m_timeOfLastSentGuaranteedMessage = newTimeSeconds;
	}

	double Client::getLastHeardFromTime() const
	{
		return m_lastHeardFromTime;
	}

	double Client::getTimeOfLastSentRTT() const
	{
		return m_timeOfLastSentRTT;
	}

	double Client::getRTT() const
	{
		return m_currentRTT;
	}
}