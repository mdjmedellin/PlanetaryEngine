#include "GeneralCommand.hpp"

namespace gh
{
	GeneralCommand::GeneralCommand( PacketCommandType typeOfCommand, unsigned int cmdID /*= 0*/ )
		:	Message( typeOfCommand )
		,	m_commandID( cmdID )
	{}

	GeneralCommand::GeneralCommand()
		:	Message( CMD_NONE )
		,	m_commandID( 0 )
	{}

	void GeneralCommand::serialize( std::vector< char >& buffer )
	{
		Message::serialize( buffer );
		writeToBuffer( m_commandID, buffer );
	}

	void GeneralCommand::unserialize( const char* buffer, size_t& index )
	{
		Message::unserialize( buffer, index );
		readFromBuffer( m_commandID, buffer, index );
	}
}