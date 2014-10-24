#ifndef UPDATE_ACTOR_COMMAND_H
#define UPDATE_ACTOR_COMMAND_H
#pragma once

#include "Message.hpp"
#include "ActorData.h"

namespace gh
{
	class UpdateActorCommand : public Message
	{
	public:
		ActorData m_actorData;

		UpdateActorCommand();
		UpdateActorCommand( PacketCommandType commandType, const ActorData& actorInformation );

		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize( const char* buffer, size_t& index );
	};
}

#endif