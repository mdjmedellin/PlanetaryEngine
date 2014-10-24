
#include "EventSystem.h"

gh::EventSystem theEventSystem;

namespace gh
{
	void EventSystem::FireEvent( const std::string& eventName )
	{
		NamedProperties emptyNamedProperties;
		FireEvent( eventName, emptyNamedProperties );
	}

	void EventSystem::FireEvent( const std::string& eventName, const NamedProperties& eventArguments )
	{
		std::map< std::string, std::vector< SubscriberBase* > >::iterator found = m_registry.find( eventName );

		if( found != m_registry.end() )
		{
			std::for_each( found->second.begin(), found->second.end(), [&]( SubscriberBase* currentSubscriber )
			{
				currentSubscriber->callRegisteredFunc( eventArguments );
			});
		}
	}
}