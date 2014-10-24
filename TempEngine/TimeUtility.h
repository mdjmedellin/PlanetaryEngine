#ifndef TIME_UTILITY_H
#define TIME_UTILITY_H
#pragma once

namespace gh
{
	typedef unsigned long long SystemClocks;

	void InitializeTimeSystem();

	SystemClocks GetAbsoluteTimeClocks();
	SystemClocks GetClockFrequency();
	double GetClockFrequencyDouble();
	double GetAbsoluteTimeSeconds();

	double ClocksToSeconds( SystemClocks clocks );
	SystemClocks SecondsToClocks( double seconds );
}

#endif