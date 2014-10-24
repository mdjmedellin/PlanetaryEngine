#include "UpdateInputCommand.hpp"

namespace gh
{
	UpdateInputCommand::UpdateInputCommand( double timeOffset, const InputState& newInputState )
		:	Message( CMD_UPDATE_INPUT )
		,	m_inputState( newInputState )
		,	m_timeOffset( timeOffset )
	{}

	UpdateInputCommand::UpdateInputCommand()
		:	Message( CMD_UPDATE_INPUT )
		,	m_inputState( InputState() )
		,	m_timeOffset( 0.0 )
	{}

	void UpdateInputCommand::serialize( std::vector< char >& buffer )
	{
		Message::serialize( buffer );
		writeToBuffer( m_inputState, buffer );
		writeToBuffer( m_timeOffset, buffer );
	}

	void UpdateInputCommand::unserialize( const char* buffer, size_t& index )
	{
		Message::unserialize( buffer, index );
		readFromBuffer( m_inputState, buffer, index );
		readFromBuffer( m_timeOffset, buffer, index );
	}
}