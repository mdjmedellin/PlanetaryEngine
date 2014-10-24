#include "MutexLock.h"
#include "Mutex.h"

namespace gh
{
	MutexLock::MutexLock( Mutex& mutexObject )
		:	m_mutexObject( &mutexObject )
	{
		m_mutexObject->lock();
	}

	MutexLock::~MutexLock()
	{
		m_mutexObject->unlock();
		m_mutexObject = nullptr;
	}
}