#include "ClientCommand.hpp"

namespace gh
{
	void ClientCommand::serialize( std::vector< char >& buffer )
	{
		Message::serialize( buffer );
	}

	void ClientCommand::unserialize( const char* buffer, size_t& index )
	{
		Message::unserialize( buffer, index );
	}

	ClientCommand::ClientCommand( PacketCommandType typeOfCommand )
		:	Message( typeOfCommand )
	{}

}