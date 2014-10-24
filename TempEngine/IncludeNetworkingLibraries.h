#ifndef INCLUDE_NETWORKING_LIBRARIES_H
#define INCLUDE_NETWORKING_LIBRARIES_H
#pragma once

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#endif

#pragma comment( lib, "Ws2_32.lib" )
#include <winsock2.h>

namespace gh
{
	int initializeNetworking( const WORD& wsaVersion );
	void endNetworking();

	static WSADATA wsaData;
}

#endif