#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H
#pragma once

#include <map>

namespace gh
{
	class NetworkingObject;

	class SocketManager
	{
	public:
		bool createNewSocket( const std::string& socketLabel, unsigned short portNumber = 0, const std::string& ipAddress = "" );
		NetworkingObject* getSocketInstance( const std::string& socketLabel );
		bool removeSocket( const std::string& socketLabel );

		static void create();
		static void destroy();
		static SocketManager& instance();

	private:
		std::map< const std::string, NetworkingObject* > m_openedSockets;
		static SocketManager* s_instance;
	};
}

#endif