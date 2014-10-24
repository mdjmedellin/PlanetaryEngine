#include "PacketHeader.h"

namespace gh
{
	PacketHeader::PacketHeader( const PacketCommandType& packetType )
		:	m_type( packetType )
	{}

	void PacketHeader::reset( const PacketCommandType& packetType /*= CMD_NONE */ )
	{
		m_type = packetType;
	}

	PacketCommandType PacketHeader::getType()
	{
		return m_type;
	}

	unsigned long long PacketHeader::getPacketNumber()
	{
		return m_packetNumber;
	}

}