#ifndef ADD_TIMELINE_ACTOR_COMMAND_H
#define ADD_TIMELINE_ACTOR_COMMAND_H

#include "Message.hpp"
#include "SpaceGameUtilities.hpp"

namespace gh
{
	class AddTimelineActorCommand : public Message
	{
	public:

		AddTimelineActorCommand();
		AddTimelineActorCommand( PacketCommandType typeOfCommand, double timeOfState, TimelineState actorState );

		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize( const char* buffer, size_t& index );

		double m_timeOfState;
		TimelineState m_actorState;
	};
}

#endif