#ifndef DEATH_COMMAND_H
#define DEATH_COMMAND_H
#pragma once

#include "Message.hpp"

namespace gh
{
	class DeathCommand : public Message
	{
	public:
		unsigned int m_deadActorID;

		DeathCommand();
		DeathCommand( unsigned int deadActorID );

		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize( const char* buffer, size_t& index );
	};
}

#endif