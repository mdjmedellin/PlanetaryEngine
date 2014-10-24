#ifndef NETWORKING_OBJECT_H
#define NETWORKING_OBJECT_H
#pragma once

#include "IncludeNetworkingLibraries.h"
#include <string>
#include "NetworkPacket.h"
#include <fstream>
#include <vector>

namespace gh
{
	class NetworkingObject
	{
	public:
		NetworkingObject();
		~NetworkingObject();

		void closeSocket();
		void initializeSocket();
		int bindSocket( unsigned short port = 0, const std::string& ipAddress = "" );
		unsigned long getBytesQueued();
		int receivePacket();
		void resetPacket( const PacketCommandType& typeOfPacket = CMD_NONE, const char* packetData = nullptr, size_t dataSize = 0 );
		PacketCommandType getPacketType();
		bool getPacketNumber( unsigned long long& out_packetNumber );
		int sendPacketTo( const sockaddr_in& sendAddr );
		int sendPacket();
		bool writeToPacket( const char* dataToWrite, size_t sizeOfData );
		void readPacketContents( char* dataBuffer, size_t sizeOfBuffer );
		unsigned long getSenderAddress();
		unsigned short getSenderPortNumber();
		void sendPacketToSender();
		sockaddr_in getSenderSockAddr() const;
		PacketCommandType getCurrentCommandType() const;
		PacketCommandType moveToNextCommand();

		std::vector< char > m_messageBuffer;
		size_t m_messageIndex;
		char* m_receivedData;

	private:
		void moveToFirstCommand();

		int iResult;
		int m_bytesReceived;
		SOCKET m_socket;
		sockaddr_in m_address;
		unsigned long m_bytesQueued;
		int m_bufferSize;
		NetworkPacket* m_currentPacket;
		sockaddr_in m_senderAddress;
		int m_senderAddressSize;
		char* m_buffer;
		PacketCommandType m_currentCommand;
		std::ofstream outputFile;
	};
}

#endif

