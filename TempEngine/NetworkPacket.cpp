#include "NetworkPacket.h"
#include <xutility>

namespace gh
{
	NetworkPacket::NetworkPacket( const PacketCommandType& packetType /*= CMD_NONE */ )
		:	m_header( PacketHeader( packetType ) )
		,	m_end( 0 )
	{
		std::fill_n( m_data, MAX_DATA_SIZE, '/0' );
	}

	void NetworkPacket::reset( const PacketCommandType& typeOfPacket /*= CMD_NONE */ )
	{
		m_header.reset( typeOfPacket );
		std::fill_n( m_data, MAX_DATA_SIZE, '/0' );
		m_end = 0;
	}

	PacketCommandType NetworkPacket::getType()
	{
		return m_header.getType();
	}

	bool NetworkPacket::writeToBuffer( const char* byteArray, int byteArraySize )
	{
		if( m_end + byteArraySize < MAX_DATA_SIZE )
		{
			memcpy( &m_data[ m_end ], byteArray, byteArraySize );
			m_end += byteArraySize;
			return true;
		}
		else
		{
			return false;
		}
	}

	void NetworkPacket::readFromBuffer( char* byteArray, int byteArraySize )
	{
		int bytesToCopy = MAX_DATA_SIZE;
		if( byteArraySize < MAX_DATA_SIZE )
		{
			bytesToCopy = byteArraySize;
		}

		memcpy( byteArray, m_data, bytesToCopy );
	}

	unsigned long long NetworkPacket::getPacketNumber()
	{
		return m_header.getPacketNumber();
	}

}