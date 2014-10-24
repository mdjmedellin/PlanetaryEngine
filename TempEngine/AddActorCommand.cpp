#include "AddActorCommand.hpp"

namespace gh
{
	void AddActorCommand::serialize( std::vector< char >& buffer )
	{
		Message::serialize(	buffer );
		writeToBuffer( m_actorData, buffer );
	}

	void AddActorCommand::unserialize( const char* buffer, size_t& index )
	{
		//command types are read outside of the function
		Message::unserialize( buffer, index );
		readFromBuffer( m_actorData, buffer, index );
	}

	AddActorCommand::AddActorCommand( PacketCommandType typeOfCommand, const ActorData& actorInformation )
		:	Message( typeOfCommand )
		,	m_actorData( actorInformation )
	{}

	AddActorCommand::AddActorCommand()
		:	Message( CMD_NONE )
	{}

}