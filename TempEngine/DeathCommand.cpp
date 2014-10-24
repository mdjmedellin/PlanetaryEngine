#include "DeathCommand.hpp"

namespace gh
{
	void DeathCommand::serialize( std::vector< char >& buffer )
	{
		Message::serialize( buffer );
		writeToBuffer( m_deadActorID, buffer );
	}

	void DeathCommand::unserialize( const char* buffer, size_t& index )
	{
		Message::unserialize( buffer, index );
		readFromBuffer( m_deadActorID, buffer, index );
	}

	DeathCommand::DeathCommand()
		:	Message( CMD_NONE )
		,	m_deadActorID( 0 )
	{}

	DeathCommand::DeathCommand( unsigned int deadActorID )
		:	Message( CMD_REMOVE_ACTOR )
		,	m_deadActorID( deadActorID )
	{}

}