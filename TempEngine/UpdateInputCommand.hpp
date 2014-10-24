#ifndef UPDATE_INPUT_COMMAND_H
#define UPDATE_INPUT_COMMAND_H
#pragma once

#include "Message.hpp"
#include "SpaceGameUtilities.hpp"

namespace gh
{
	class UpdateInputCommand : public Message
	{
	public:
		InputState m_inputState;
		double m_timeOffset;

		UpdateInputCommand();
		UpdateInputCommand( double timeOffset, const InputState& newInputState );

		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize( const char* buffer, size_t& index );
	};
}

#endif