#include "UpdateActorCommand.hpp"

namespace gh
{
	void UpdateActorCommand::serialize( std::vector< char >& buffer )
	{
		Message::serialize( buffer );
		writeToBuffer( m_actorData, buffer );
	}

	void UpdateActorCommand::unserialize( const char* buffer, size_t& index )
	{
		Message::unserialize( buffer, index );
		readFromBuffer( m_actorData, buffer, index );
	}

	UpdateActorCommand::UpdateActorCommand()
		:	Message( CMD_NONE )
		,	m_actorData( ActorData() )
	{}

	UpdateActorCommand::UpdateActorCommand( PacketCommandType commandType, const ActorData& actorInformation )
		:	Message( commandType )
		,	m_actorData( actorInformation )
	{}

}