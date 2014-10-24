#pragma once
#ifndef MUTEX_LOCK_H
#define MUTEX_LOCK_H

namespace gh
{
	//forward declarations
	class Mutex;
	//

	class MutexLock
	{
	public:
		MutexLock( Mutex& mutexObject );
		~MutexLock();

	private:
		Mutex* m_mutexObject;
	};
}

#endif