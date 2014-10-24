#include "AddTimelineActorCommand.hpp"

namespace gh
{
	AddTimelineActorCommand::AddTimelineActorCommand()
		:	Message( CMD_NONE )
		,	m_timeOfState( 0.0 )
		,	m_actorState( TimelineState() )
	{}

	AddTimelineActorCommand::AddTimelineActorCommand( PacketCommandType typeOfCommand, double timeOfState, TimelineState actorState )
		:	Message( typeOfCommand )
		,	m_timeOfState( timeOfState )
		,	m_actorState( actorState )
	{}

	void AddTimelineActorCommand::serialize( std::vector< char >& buffer )
	{
		Message::serialize( buffer );

		writeToBuffer( m_timeOfState, buffer );
		writeToBuffer( m_actorState, buffer );
	}

	void AddTimelineActorCommand::unserialize( const char* buffer, size_t& index )
	{
		Message::unserialize( buffer, index );

		readFromBuffer( m_timeOfState, buffer, index );
		readFromBuffer( m_actorState, buffer, index );
	}

}