#ifndef LOGIN_COMMAND_H
#define LOGIN_COMMAND_H
#pragma once

namespace gh
{
	class LoginCommand : Message
	{
		virtual void serialize( std::vector< char >& buffer );
		virtual void unserialize( const char* buffer, size_t& index );
	};
}

#endif