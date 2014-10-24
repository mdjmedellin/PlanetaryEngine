#ifndef UPDATE_TIMELINE_STATE_COMMAND_H
#define UPDATE_TIMELINE_STATE_COMMAND_H

#include "Message.hpp"
#include "SpaceGameUtilities.hpp"

namespace gh
{
	class UpdateTimelineStateCommand : public Message
	{
	public:

		UpdateTimelineStateCommand( PacketCommandType commandType = CMD_NONE );
		UpdateTimelineStateCommand( PacketCommandType typeOfCommand, double timeOfState, TimelineState actorState );

		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize( const char* buffer, size_t& index );

		double m_timeOfState;
		TimelineState m_actorState;
	};
}

#endif