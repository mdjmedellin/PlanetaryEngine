#pragma once
#ifndef SELF_UNREGISTER_H
#define SELF_UNREGISTER_H

#include "EventSystem.h"

namespace gh
{
	class SelfUnregister
	{
	public:
		virtual ~SelfUnregister()
		{
			theEventSystem.UnregisterDerivedObject( this );
		}
	};
}

#endif