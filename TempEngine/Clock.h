#pragma once
#ifndef CLOCK_H
#define CLOCK_H

//includes
#include <vector>
//

namespace gh
{
	typedef unsigned long long milliseconds;
	typedef unsigned long long microseconds;

	class NamedProperties;
	class Alarm;

	class Clock
	{
	public:
		void setMaximumDelta( double maximumDeltaTimeSeconds );
		void togglePause();
		void setTimeScale( float timeScale );
		void addAlarm( const std::string& eventName, const NamedProperties& eventArguments, double alarmTimeSeconds );
		double getTimeSeconds();
		static std::weak_ptr< Clock > addClock( std::weak_ptr< Clock > parentClock, float timeScale = 1.f );
		static void createMasterClock();
		static void destroyMasterClock();
		static std::weak_ptr< Clock > getMasterClock();
		static bool removeClock( std::weak_ptr< Clock > clockToRemove );
		static void advanceTime( double deltaSeconds, bool printTime = false );
		static void addTimeSinceLastUpdate();

	private:
		Clock();
		Clock( std::weak_ptr< Clock > parentClock, float timeScale = 1.f );
		void checkAlarms();
		bool removeChild( std::weak_ptr< Clock > childToRemove );
		bool removeAllChildren();
		void advanceClockTime( double deltaSeconds, bool printTime = false );
		void advanceChildrenTimes( double deltaSeconds, bool printTime = false );

		//milliseconds m_currentTimeMilliseconds;
		microseconds m_currentTimeMicroseconds;
		//milliseconds m_previousTimeDeltaMilliseconds;
		microseconds m_previousTimeDeltaMicroseconds;
		//milliseconds m_maximumDeltaMilliseconds;
		microseconds m_maximumDeltaMicroseconds;
		double m_timeScale;
		bool m_isPaused;
		std::weak_ptr< Clock > m_parent;
		std::vector< std::shared_ptr< Clock > > m_childrenClocks;
		std::vector< std::shared_ptr< Alarm > > m_alarms;

		static std::shared_ptr< Clock > s_instance;
		static double m_timeOfLastUpdate;
		/*static std::vector< std::weak_ptr< Clock > > s_vectorOfLiveClocks;*/
	};
}

#endif