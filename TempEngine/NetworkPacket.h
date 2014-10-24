#ifndef NETWORK_PACKET_H
#define NETWORK_PACKET_H
#pragma once

#include "PacketHeader.h"

namespace
{
	static const int MAX_DATA_SIZE = 512;
}

namespace gh
{
	class NetworkPacket
	{
	public:
		NetworkPacket( const PacketCommandType& packetType = CMD_NONE );

		void reset( const PacketCommandType& typeOfPacket = CMD_NONE );
		PacketCommandType getType();
		unsigned long long getPacketNumber();
		bool writeToBuffer( const char* byteArray, int byteArraySize );
		void readFromBuffer( char* byteArray, int byteArraySize );

	private:
		PacketHeader m_header;
		char m_data[ MAX_DATA_SIZE ];
		int m_end;
	};
}

#endif