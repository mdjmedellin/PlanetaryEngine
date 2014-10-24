#pragma once
#ifndef DEBUG_UTILITY_H
#define DEBUG_UTILITY_H

//includes
#include <string>
//

namespace gh
{
	void traceString( const std::string& message );

#define trace( message )				\
	{									\
		std::ostringstream s;			\
		s << message << std::endl;		\
		gh::traceString( s.str() );		\
	}
}

#endif