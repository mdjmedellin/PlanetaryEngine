#pragma once
#ifndef MUTEX_H
#define MUTEX_H

#include <Windows.h>

namespace gh
{
	class Mutex
	{
	public:
		Mutex();
		~Mutex();

		void lock();
		void unlock();
		int getRecursionCount() const;

	private:
		CRITICAL_SECTION m_criticalSection;
		int m_recurseCount;
	};
}

#endif