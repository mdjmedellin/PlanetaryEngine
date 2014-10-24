#ifndef CLIENT_COMMAND_H
#define CLIENT_COMMAND_H

#include "Message.hpp"

namespace gh
{
	class ClientCommand : Message
	{
	public:
		ClientCommand( PacketCommandType typeOfCommand );

		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize(  const char* buffer, size_t& index );
	};
}

#endif