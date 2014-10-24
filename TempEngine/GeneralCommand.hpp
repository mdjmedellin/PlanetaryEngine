#ifndef GENERAL_COMMAND_H
#define GENERAL_COMMAND_H

#include "Message.hpp"

namespace gh
{
	class GeneralCommand : public Message
	{
	public:
		unsigned int m_commandID;

		GeneralCommand();
		GeneralCommand( PacketCommandType typeOfCommand, unsigned int cmdID = 0 );

		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize( const char* buffer, size_t& index );
	};
}

#endif