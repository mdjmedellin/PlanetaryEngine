#include "Mutex.h"

namespace gh
{
	Mutex::Mutex()
		:	m_recurseCount( 0 )
	{
		InitializeCriticalSection( &m_criticalSection );
	}

	Mutex::~Mutex()
	{
		DeleteCriticalSection( &m_criticalSection );
	}

	void Mutex::lock()
	{
		EnterCriticalSection( &m_criticalSection ); // in lock
		++m_recurseCount;
	}

	void Mutex::unlock()
	{
		--m_recurseCount;
		LeaveCriticalSection( &m_criticalSection );
	}

	int Mutex::getRecursionCount() const
	{
		return m_recurseCount;
	}
}