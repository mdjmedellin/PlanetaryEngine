#include "NetworkingObject.h"
#include "NetworkPacket.h"
#include <winsock.h>
#include "TimeUtility.h"

namespace gh
{
	NetworkingObject::NetworkingObject()
		:	m_bytesQueued( 0 )
		,	iResult( 0 )
		,	m_currentPacket( nullptr )
		,	m_socket( INVALID_SOCKET )
		,	m_currentCommand( CMD_NONE )
		,	m_messageIndex( 0 )
		,	m_bytesReceived( 0 )
	{
		//m_bufferSize = sizeof( NetworkPacket );
		m_bufferSize = 4096;			//This is = to .5 kb
		m_receivedData = new char[ m_bufferSize ];
		m_messageBuffer.reserve( m_bufferSize );

		//m_currentPacket = new( m_buffer ) NetworkPacket();
		//m_buffer and m_currentPacket both point to the same location in memory

		m_senderAddressSize = sizeof( m_senderAddress );
		initializeSocket();

		outputFile.open ("test.txt", std::ofstream::out | std::ofstream::app);
		outputFile <<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	}

	NetworkingObject::~NetworkingObject()
	{
		//TODO
		//Cleanup the two dynamic arrays that are created
		//m_buffer && NetworkPacket
		outputFile.close();
		closeSocket();
	}

	void NetworkingObject::closeSocket()
	{	
		printf( "Closing Socket\n" );
		iResult = closesocket( m_socket );
		if ( iResult == SOCKET_ERROR )
		{
			printf( "ERROR: Could not close socket\n" );
		}
	}

	void NetworkingObject::initializeSocket()
	{
		m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
		if( m_socket == INVALID_SOCKET )
		{
			printf( "ERROR: could not create socket\n" );
			iResult = -1;
		}
		else
		{
			printf( "SOCKET CREATED\n" );
			unsigned long enabled = 1;
			ioctlsocket( m_socket, FIONBIO, &enabled );
			iResult = 0;
		}
	}

	int NetworkingObject::bindSocket( unsigned short port /* = 0 */, const std::string& ipAddress /* = "" */ )
	{
		m_address.sin_family = AF_INET;
		m_address.sin_port = htons( port );

		if( ipAddress.empty() )
		{
			//we know that we should use any address
			m_address.sin_addr.s_addr = htonl( INADDR_ANY );
		}
		else
		{
			m_address.sin_addr.s_addr = inet_addr( ipAddress.c_str() );
		}

		iResult = bind( m_socket, ( SOCKADDR* )& m_address, sizeof( m_address ) );
		if( iResult != 0 )
		{
			printf( "ERROR: Could not bind socket to the specified location\n" );
		}
		else
		{
			printf( "SOCKET BOUND\n" );
		}

		return iResult;
	}

	unsigned long NetworkingObject::getBytesQueued()
	{
		//check if there are new packets available
		ioctlsocket( m_socket, FIONREAD, &m_bytesQueued );

		return m_bytesQueued;
	}

	void NetworkingObject::moveToFirstCommand()
	{
		m_currentCommand = PacketCommandType( *( int* )&m_receivedData[0] );

		if( m_currentCommand != CMD_LOGIN )
		{
			int x = 1;
		}

		m_messageIndex = sizeof( int );
	}

	int NetworkingObject::receivePacket()
	{
		m_currentCommand = CMD_NONE;
		m_messageIndex = 0;

		if( getBytesQueued() > 0 )
		{

			m_bytesReceived = recvfrom( m_socket, m_receivedData, m_bufferSize, 0, ( SOCKADDR* ) &m_senderAddress, 
								&m_senderAddressSize );

			if( m_bytesReceived == SOCKET_ERROR )
			{
				//printf( "Error receiving packet \n" );
				return SOCKET_ERROR;
			}
			else
			{
				moveToFirstCommand();
				//outputFile << "Received Package of type " << m_currentCommand << " at time: " << GetAbsoluteTimeClocks() << " from " <<  m_senderAddress.sin_port <<"\n";
				return 0;
			}
		}
		else
		{
			return SOCKET_ERROR;
		}
	}

	void NetworkingObject::resetPacket( const PacketCommandType& typeOfPacket /* = CMD_NONE */, const char* packetData /* = nullptr */, size_t sizeOfData /* = 0 */ )
	{
		m_currentPacket->reset( typeOfPacket );

		if( packetData != nullptr && sizeOfData != 0 )
		{
			m_currentPacket->writeToBuffer( packetData, sizeOfData );
		}
	}

	PacketCommandType NetworkingObject::getPacketType()
	{
		if( m_currentPacket )
		{
			return m_currentPacket->getType();
		}
		else
		{
			return CMD_NONE;
		}
	}

	bool NetworkingObject::getPacketNumber( unsigned long long& out_packetNumber )
	{
		if( m_currentPacket )
		{
			out_packetNumber = m_currentPacket->getPacketNumber();
			return true;
		}
		else
		{
			return false;
		}
	}

	int NetworkingObject::sendPacketTo( const sockaddr_in& sendAddr )
	{
		if( !m_messageBuffer.empty() )
		{
			//allocate the data into a temporary char buffer and send that
			char* tempSendBuffer;
			tempSendBuffer = new char[ m_messageBuffer.size() ];
			int sizeOfVector = m_messageBuffer.size();
			memcpy( tempSendBuffer, &( *m_messageBuffer.begin() ), m_messageBuffer.size() );

			iResult = sendto( m_socket, tempSendBuffer, m_messageBuffer.size(), 0, ( SOCKADDR *) &sendAddr, sizeof( sendAddr ) );

			delete tempSendBuffer;
			m_messageBuffer.clear();

			if( iResult == SOCKET_ERROR )
			{
				iResult = WSAGetLastError();
				//printf( "ERROR: Could not send message %i\n", iResult );
				return iResult;
			}
			else
			{
				//outputFile << "Sending Package of type UNKNOWN at time: " << GetAbsoluteTimeClocks() << " to " <<  sendAddr.sin_port <<"\n";
				return 0;
			}
		}
		else
		{
			return -1;
		}
	}

	int NetworkingObject::sendPacket()
	{
		return sendPacketTo( m_senderAddress );
	}

	bool NetworkingObject::writeToPacket( const char* dataToWrite, size_t sizeOfData )
	{
		if( m_currentPacket )
		{
			return m_currentPacket->writeToBuffer( dataToWrite, sizeOfData );
		}
		else
		{
			return false;
		}
	}

	void NetworkingObject::readPacketContents( char* dataBuffer, size_t sizeOfBuffer )
	{
		if( m_currentPacket )
		{
			m_currentPacket->readFromBuffer( dataBuffer, sizeOfBuffer );
		}
	}

	unsigned long NetworkingObject::getSenderAddress()
	{
		return m_senderAddress.sin_addr.S_un.S_addr;
	}

	unsigned short NetworkingObject::getSenderPortNumber()
	{
		return m_senderAddress.sin_port;
	}

	void NetworkingObject::sendPacketToSender()
	{
		sendPacketTo( m_senderAddress );
	}

	sockaddr_in NetworkingObject::getSenderSockAddr() const
	{
		return m_senderAddress;
	}

	PacketCommandType NetworkingObject::getCurrentCommandType() const
	{
		return m_currentCommand;
	}

	PacketCommandType NetworkingObject::moveToNextCommand()
	{
		if( m_messageIndex == m_bytesReceived )
		{
			m_currentCommand = CMD_NONE;
		}
		else
		{
			m_currentCommand = PacketCommandType( *( int* )&m_receivedData[ m_messageIndex ] );
			m_messageIndex += sizeof( int );
		}

		return m_currentCommand;
	}

}
