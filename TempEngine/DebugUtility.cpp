#include "DebugUtility.h"
#include <windows.h>

namespace gh
{
	void traceString( const std::string& message )
	{
#ifdef _WIN32
		OutputDebugStringA( message.c_str() );
#else
		std::cerr<< message;
#endif
	}
}