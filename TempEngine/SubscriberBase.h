#pragma once
#ifndef SUBSCRIBER_BASE_H
#define SUBSCRIBER_BASE_H

namespace gh
{
	class NamedProperties;

	class SubscriberBase
	{
	public:
		virtual void callRegisteredFunc( const NamedProperties& arguments ) = 0;
		virtual void* getVoidPtrObject() = 0;
	};
}

#endif