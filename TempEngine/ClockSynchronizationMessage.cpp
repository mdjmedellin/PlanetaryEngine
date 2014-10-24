#include "ClockSynchronizationMessage.hpp"

namespace gh
{


	ClockSynchronizationMessage::ClockSynchronizationMessage()
		:	Message( CMD_SYNCHRONIZE_CLOCK )
		,	m_currentTime( 0.0 )
		,	m_nextUpdateTime( 0.0 )
	{}

	ClockSynchronizationMessage::ClockSynchronizationMessage( double nextUpdateTime, double currentTime )
		:	Message( CMD_SYNCHRONIZE_CLOCK )
		,	m_currentTime( currentTime )
		,	m_nextUpdateTime( nextUpdateTime )
	{}

	void ClockSynchronizationMessage::serialize( std::vector< char >& buffer )
	{
		Message::serialize( buffer );
		writeToBuffer( m_currentTime, buffer );
		writeToBuffer( m_nextUpdateTime, buffer );
	}

	void ClockSynchronizationMessage::unserialize( const char* buffer, size_t& index )
	{
		Message::unserialize( buffer, index );
		readFromBuffer( m_currentTime, buffer, index );
		readFromBuffer( m_nextUpdateTime, buffer, index );
	}

}