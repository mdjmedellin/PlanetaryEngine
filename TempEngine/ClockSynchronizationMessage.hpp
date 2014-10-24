#ifndef CLOCK_SYNCHRONIZATION_MESSAGE
#define CLOCK_SYNCHRONIZATION_MESSAGE
#pragma once

#include "Message.hpp"

namespace gh
{
	class ClockSynchronizationMessage : public Message
	{
	public:
		ClockSynchronizationMessage();
		ClockSynchronizationMessage( double nextUpdateTime, double currentTime );

		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize( const char* buffer, size_t& index );

		double m_nextUpdateTime;
		double m_currentTime;
	};
}

#endif