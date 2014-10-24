#include "Message.hpp"

namespace gh
{
	Message::Message( PacketCommandType typeOfCommand )
		:	m_commandType( typeOfCommand )
		,	m_packetID( s_nextMessageID++ )
		,	m_isGuaranteed( false )
		,	m_isAcknowledged( false )
		,	subscribers( 0 )
	{}

	void Message::serialize( std::vector< char >& buffer )
	{
		writeToBuffer( m_commandType, buffer );
		writeToBuffer( m_packetID, buffer );
		writeToBuffer( m_isGuaranteed, buffer );
		writeToBuffer( m_isAcknowledged, buffer );
	}

	void Message::unserialize( const char* buffer, size_t& index )
	{
		readFromBuffer( m_packetID, buffer, index );
		readFromBuffer( m_isGuaranteed, buffer, index );
		readFromBuffer( m_isAcknowledged, buffer, index );
	}

	unsigned int Message::getPacketID() const
	{
		return m_packetID;
	}

	void Message::acknowledgePacket()
	{
		m_isAcknowledged = true;
	}

	void Message::setAsGuaranteed()
	{
		m_isGuaranteed = true;
	}

	bool Message::isAcknowledged() const
	{
		return m_isAcknowledged;
	}

	bool Message::isGuaranteed() const
	{
		return m_isGuaranteed;
	}

}