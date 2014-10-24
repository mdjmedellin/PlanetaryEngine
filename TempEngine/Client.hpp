#ifndef CLIENT_H
#define CLIENT_H

#include "TimeUtility.h"
#include "IncludeNetworkingLibraries.h"
#include <vector>
#include <map>

namespace gh
{
	namespace SpaceCombat
	{
		class Player;
	}
	class Message;
	class NetworkingObject;
	class ActorData;

	class Client
	{
	public:
		Client( unsigned short portNumber, unsigned long address, const sockaddr_in& socketAddress );
		Client();
		~Client();

		void updateLastHeardFromTime( double newTimeSeconds );
		double getLastHeardFromTime() const;
		void updateTimeOfLastGuaranteedMessage( double newTimeSeconds );
		double getTimeOfLastSentGuaranteedMessage() const;
		double getTimeOfLastSentRTT() const;
		double getRTT() const;

		void queueRTTMessage();
		void processRTT( unsigned int rttMessageIndex );
		void sendMessage( NetworkingObject* connection );
		void addGuaranteedMessage( Message* guaranteedMessage );
		bool hasProcessedAllGuaranteedMessages() const;
		void sendGuaranteedMessages( NetworkingObject* connection );
		void sendAcknowledgements( NetworkingObject* connection );
		void acknowledgeMessage( unsigned int messageID );
		bool actorIsAlive() const;
		void updateActor( const ActorData* updatedActorData );
		void setAsDropped();
		bool isDropped() const;
		void killPlayer( bool respawnAfterKill = false );
		bool hasPlayerID( unsigned long playerID );
		bool respawnPlayer();

		long long m_currentPacketNumber;
		SpaceCombat::Player* m_player;
		std::vector< Message* > m_messageQueue;
		std::vector< Message* > m_acknowledgementQueue;
		unsigned short m_port;
		unsigned long m_address;
		sockaddr_in m_socketAddress;
		bool m_newGuaranteedMessage;
		bool m_isInitialized;

	private:
		double m_lastHeardFromTime, m_timeOfLastSentGuaranteedMessage;
		unsigned int m_RTTCounter;
		bool m_RTTArrived;
		double m_currentRTT;
		double m_timeOfLastSentRTT;
		std::vector< Message* > m_lastMessageQueue;
		std::map< unsigned int, Message* > m_guaranteedMessageMap;
		bool m_clientIsDropped;
	};
}

#endif