#pragma once
#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include "SubscriberBase.h"

namespace gh
{
	class NamedProperties;

	template< typename T_dataType >
	class Subscriber : public SubscriberBase
	{	
	public:
		typedef void( T_dataType::*ObjectMemberFuncType )( const NamedProperties& arguments );

		Subscriber( T_dataType* object, ObjectMemberFuncType funct )
			:	m_subscriberObject( object )
			,	m_callbackMethod( funct )
		{}

		virtual void callRegisteredFunc( const NamedProperties& arguments )
		{
			( m_subscriberObject->*m_callbackMethod )( arguments );
		}

		virtual void* getVoidPtrObject()
		{
			return ( void* )( m_subscriberObject );
		}

		T_dataType* m_subscriberObject;

		ObjectMemberFuncType m_callbackMethod;
	};
}

#endif