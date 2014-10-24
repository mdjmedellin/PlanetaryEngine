#ifndef ADD_ACTOR_COMMAND_H
#define ADD_ACTOR_COMMAND_H

#include "Message.hpp"
#include "ActorData.h"

namespace gh
{
	class AddActorCommand : public Message
	{
	public:
		ActorData m_actorData;

		AddActorCommand();
		AddActorCommand( PacketCommandType typeOfCommand, const ActorData& actorInformation );

		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize( const char* buffer, size_t& index );
	};
}

#endif