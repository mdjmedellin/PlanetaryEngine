#ifndef MESSAGE_H
#define MESSAGE_H
#pragma once

#include <vector>
#include "PacketHeader.h"

namespace gh
{
	class Message
	{
	public:
		Message( PacketCommandType typeOfCommand );
		unsigned int getPacketID() const;
		void acknowledgePacket();
		void setAsGuaranteed();
		bool isAcknowledged() const;
		bool isGuaranteed() const;

		virtual void serialize( std::vector< char >& buffer ) = 0;
		virtual void unserialize( const char* buffer, size_t& index ) = 0;

		//this variable only matters for the guaranteed messages
		unsigned subscribers;

	protected:

		template <class T>
		void readFromBuffer( T& val, const char* buffer, size_t& index )
		{
			val = *( ( T* )( &buffer[ index ] ) );
			index += sizeof( T );
		}

		template <class T>
		void writeToBuffer( const T& val, std::vector< char >& buffer )
		{
			buffer.insert( buffer.end(), ( char* )( &val ), ( char* )( &val+1 ) );
		}

		PacketCommandType m_commandType;
		unsigned int m_packetID;
		bool m_isGuaranteed;
		bool m_isAcknowledged;
	};

	static unsigned int s_nextMessageID = 0;
}

#endif