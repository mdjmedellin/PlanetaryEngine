#pragma once
#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include "Subscriber.h"
#include "NamedProperties.h"
#include "CommandConsoleSystem.h"

namespace gh
{
	class EventSystem
	{
	public:
		void FireEvent( const std::string& eventName, const NamedProperties& eventArguments );
		void FireEvent( const std::string& eventName );

		template< typename T_objectDataType >
		void Register( T_objectDataType* object, typename Subscriber< T_objectDataType >::ObjectMemberFuncType func, const std::string& eventName )
		{
			std::map< std::string, std::vector< SubscriberBase* > >::iterator found = m_registry.find( eventName );

			if( found != m_registry.end() )
			{
				std::vector< SubscriberBase* >& recipients = found->second;
				Subscriber< T_objectDataType >* newSubscriber = new Subscriber< T_objectDataType >( object, func );
				recipients.push_back( newSubscriber );
			}
			else
			{
				std::vector< SubscriberBase* > recipients;
				Subscriber< T_objectDataType >* newSubscriber = new Subscriber< T_objectDataType >( object, func );
				recipients.push_back( newSubscriber );
				m_registry[ eventName ] = recipients;
			}
		}

		template< typename T_objectDataType >
		void Unregister( T_objectDataType* object, typename Subscriber< T_objectDataType >::ObjectMemberFuncType func, const std::string& eventName )
		{
			std::map< std::string, std::vector< SubscriberBase* > >::iterator found = m_registry.find( eventName );

			if( found != m_registry.end() )
			{
				std::vector< SubscriberBase* >& recipients = found->second;

				std::vector< SubscriberBase* >::iterator currentRecipient;
				for( currentRecipient = recipients.begin(); currentRecipient != recipients.end(); ++currentRecipient )
				{
					Subscriber< T_objectDataType >*currentSubscriber = dynamic_cast< Subscriber< T_objectDataType >* >( *currentRecipient );
					if( currentSubscriber )
					{
						if( currentSubscriber->m_subscriberObject == object && currentSubscriber->m_callbackMethod == func )
						{
							break;
						}
					}
				}

				if( currentRecipient != recipients.end() )
				{
					recipients.erase( currentRecipient );
				}
			}
		}

		template< typename T_objectDataType >
		void UnregisterForAll( T_objectDataType* object )
		{
			std::map< std::string, std::vector< SubscriberBase* > >::iterator currentSubscriberBase;

			for( currentSubscriberBase = m_registry.begin(); currentSubscriberBase != m_registry.end(); ++currentSubscriberBase )
			{
				std::vector< SubscriberBase* >& currentRecipients = currentSubscriberBase->second;

				std::vector< SubscriberBase* >::iterator currentRecipient;
				for( currentRecipient = currentRecipients.begin(); currentRecipient != currentRecipients.end(); )
				{
					Subscriber< T_objectDataType >*currentSubscriber = dynamic_cast< Subscriber< T_objectDataType >* >( *currentRecipient );

					if( currentSubscriber )
					{
						if( currentSubscriber->m_subscriberObject == object )
						{
							currentRecipient = currentRecipients.erase( currentRecipient );
						}
						else
						{
							++currentRecipient;
						}
					}
					else
					{
						++currentRecipient;
					}
				}
			}
		}

		template< typename T_objectDataType >
		void UnregisterDerivedObject( T_objectDataType* object )
		{
			std::map< std::string, std::vector< SubscriberBase* > >::iterator currentSubscriberBase;

			for( currentSubscriberBase = m_registry.begin(); currentSubscriberBase != m_registry.end(); ++currentSubscriberBase )
			{
				std::vector< SubscriberBase* >& currentRecipients = currentSubscriberBase->second;

				std::vector< SubscriberBase* >::iterator currentRecipient;
				for( currentRecipient = currentRecipients.begin(); currentRecipient != currentRecipients.end(); )
				{
					T_objectDataType* testObject = ( T_objectDataType* )( ( *currentRecipient )->getVoidPtrObject() );
					if( testObject && testObject == object )
					{
						currentRecipient = currentRecipients.erase( currentRecipient );
					}
					else
					{
						++currentRecipient;
					}
				}
			}
		}

	private:
		std::map< std::string, std::vector< SubscriberBase* > > m_registry;
	};
}

extern gh::EventSystem theEventSystem;

#endif