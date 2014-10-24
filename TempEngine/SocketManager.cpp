#include "SocketManager.h"
#include "NetworkingObject.h"

namespace gh
{
	bool SocketManager::createNewSocket( const std::string& socketLabel, unsigned short portNumber /*= 0*/, const std::string& ipAddress /*= "" */ )
	{
		//check if the socket is already created
		std::map< const std::string, NetworkingObject* >::iterator searchResult;
		searchResult = m_openedSockets.find( socketLabel );

		if( searchResult == m_openedSockets.end() )
		{
			//create a new socket
			m_openedSockets[ socketLabel ] = new NetworkingObject();
			if( m_openedSockets[ socketLabel ]->bindSocket( portNumber, ipAddress ) != 0 )
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		return false;
	}

	gh::NetworkingObject* SocketManager::getSocketInstance( const std::string& socketLabel )
	{
		//check if the socket is already created
		std::map< const std::string, NetworkingObject* >::iterator searchResult;
		searchResult = m_openedSockets.find( socketLabel );

		if( searchResult == m_openedSockets.end() )
		{
			return nullptr;
		}

		return searchResult->second;
	}

	bool SocketManager::removeSocket( const std::string& socketLabel )
	{
		//check if the socket is already created
		std::map< const std::string, NetworkingObject* >::iterator searchResult;
		searchResult = m_openedSockets.find( socketLabel );

		if( searchResult == m_openedSockets.end() )
		{
			return false;
		}

		delete searchResult->second;
		searchResult->second = nullptr;

		m_openedSockets.erase( searchResult );
		return true;
	}

	void SocketManager::create()
	{
		if( s_instance == nullptr )
		{
			s_instance = new SocketManager();
		}
	}

	void SocketManager::destroy()
	{
		if( s_instance != nullptr )
		{
			delete s_instance;
		}
	}

	SocketManager& SocketManager::instance()
	{
		return *s_instance;
	}

	SocketManager* SocketManager::s_instance;

}