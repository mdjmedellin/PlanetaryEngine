#include "TimeUtility.h"
#include <Windows.h>

namespace gh
{
	SystemClocks GetAbsoluteTimeClocks()
	{
		static LARGE_INTEGER start = { { 0 } };
		LARGE_INTEGER end;

		if( start.QuadPart == 0 )
		{
			::QueryPerformanceCounter( &start );
		}

		::QueryPerformanceCounter( &end );

		return ( end.QuadPart - start.QuadPart );
	}

	SystemClocks GetClockFrequency() // clocks/sec
	{
		static LARGE_INTEGER frequency= { { 0 } };

		if( frequency.QuadPart == 0 )
		{
			::QueryPerformanceFrequency( &frequency );
		}

		return frequency.QuadPart;
	}

	double GetClockFrequencyDouble()
	{
		return static_cast< double >( GetClockFrequency() );
	}

	double GetAbsoluteTimeSeconds()	// Since first call.
	{
		return ( GetAbsoluteTimeClocks() / GetClockFrequencyDouble() );
	}

	double ClocksToSeconds( SystemClocks clocks )
	{
		return ( clocks / GetClockFrequencyDouble() );
	}

	SystemClocks SecondsToClocks( double seconds )
	{
		return static_cast< SystemClocks >( seconds * GetClockFrequency() );
	}

	void InitializeTimeSystem()
	{
#ifdef _WIN32
		//don't do anything in windows
		return;
#endif
		//we might need to do some other initialization for other systems
		return;
	}

}