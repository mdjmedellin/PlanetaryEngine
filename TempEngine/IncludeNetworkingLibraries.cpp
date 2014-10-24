#include "IncludeNetworkingLibraries.h"

namespace gh
{
	int initializeNetworking( const WORD& wsaVersion )
	{
		return WSAStartup( wsaVersion, &wsaData );
	}

	void endNetworking()
	{
		WSACleanup();
	}
}