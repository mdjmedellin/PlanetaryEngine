#include <cassert>

#include "Clock.h"
#include "DebugUtility.h"
#include "Alarm.h"

#include "MathUtilities.hpp"
#include "MemoryUtilities.h"
#include "TimeUtility.h"

namespace
{
	static const double millisecondsToSecondsDivision = 1.0 / 1000.0;
	static const double microsecondsToMillisecondsDivision = millisecondsToSecondsDivision;
	static const double microsecondsToSecondsDivision = microsecondsToMillisecondsDivision * millisecondsToSecondsDivision;

	static const double secondsToMilliseconds = 1000.0;
	static const double millisecondsToMicroseconds = 1000.0;
	static const double secondsToMicroseconds = secondsToMilliseconds * millisecondsToMicroseconds;
}

namespace gh
{
	void Clock::advanceTime( double deltaSeconds, bool printTime /* = false */ )
	{
		assert( s_instance );
		s_instance->advanceClockTime( deltaSeconds, printTime );
	}

	void Clock::addTimeSinceLastUpdate()
	{
		assert( s_instance );
		s_instance->advanceClockTime( GetAbsoluteTimeSeconds() - m_timeOfLastUpdate );
		m_timeOfLastUpdate = GetAbsoluteTimeSeconds();
	}

	void Clock::advanceChildrenTimes( double deltaSeconds, bool printTime /* = false */ )
	{
		for( auto iter = m_childrenClocks.begin(); iter != m_childrenClocks.end(); ++iter )
		{
			(*iter)->advanceClockTime( deltaSeconds, printTime );
		}
	}

	Clock::Clock()
		:	m_currentTimeMicroseconds( 0 )
		,	m_previousTimeDeltaMicroseconds( 0 )
		,	m_maximumDeltaMicroseconds( microseconds( 1.0 * secondsToMicroseconds ) )
		,	m_timeScale( 1.0 )
		,	m_isPaused( false )
	{}

	Clock::Clock( std::weak_ptr< Clock > parentClock, float timeScale )
		:	m_currentTimeMicroseconds( 0 )
		,	m_previousTimeDeltaMicroseconds( 0 )
		,	m_maximumDeltaMicroseconds( microseconds( 1.0 * secondsToMicroseconds ) )
		,	m_timeScale( double( timeScale ) )
		,	m_isPaused( false )
		,	m_parent( parentClock )
	{}

	void Clock::advanceClockTime( double deltaSeconds, bool printTime /* = false */ )
	{
		if( !m_isPaused )
		{
			//scale the time by the clock's scale
			deltaSeconds *= m_timeScale;

			//convert the time to milliseconds
			microseconds deltaMicroseconds = microseconds( deltaSeconds * secondsToMicroseconds );

			if( m_maximumDeltaMicroseconds >= deltaMicroseconds )
			{
				advanceChildrenTimes( deltaSeconds, printTime );
			}
			else
			{
				deltaMicroseconds = m_maximumDeltaMicroseconds;
				advanceChildrenTimes( double( deltaMicroseconds ) * microsecondsToSecondsDivision, printTime );
			}

			m_currentTimeMicroseconds += deltaMicroseconds;
			m_previousTimeDeltaMicroseconds = deltaMicroseconds;

			checkAlarms();
		}

		//traceString( "Delta Seconds = " + std::to_string( long double( deltaSeconds ) ) + "\t" );
		//traceString( "Last Delta Time Seconds = " + std::to_string( long double ( m_previousTimeDeltaMicroseconds  * microsecondsToSecondsDivision ) ) + "\t" );
		
		if( printTime )
		{
			traceString( "Total Time Seconds = " + std::to_string( long double ( m_currentTimeMicroseconds  * microsecondsToSecondsDivision ) ) + "\t" );

			if( m_parent.expired() )
			{
				traceString( "\n" );
			}
		}
	}

	void Clock::createMasterClock()
	{
		assert( !s_instance );
		s_instance.reset( new Clock );
		m_timeOfLastUpdate = GetAbsoluteTimeSeconds();
	}

	void Clock::destroyMasterClock()
	{
		assert( s_instance );

		//other cleanup stuff
		s_instance->removeAllChildren();
		//

		s_instance.reset();
	}

	std::weak_ptr< Clock > Clock::getMasterClock()
	{
		assert( s_instance );
		return s_instance;
	}

	std::weak_ptr< Clock > Clock::addClock( std::weak_ptr< Clock > parentClock, float timeScale )
	{
		std::shared_ptr< Clock > newChildClock( new Clock( parentClock, timeScale ) );
		parentClock.lock()->m_childrenClocks.push_back( newChildClock );
		return newChildClock;
	}

	bool Clock::removeClock( std::weak_ptr< Clock > clockToRemove )
	{
		//find the clock
		if( !clockToRemove.expired() && !clockToRemove.lock()->m_parent.expired() )
		{
			clockToRemove.lock()->m_parent.lock()->removeChild( clockToRemove );
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Clock::removeChild( std::weak_ptr< Clock > childToRemove )
	{
		for( auto currentChild = m_childrenClocks.begin(); currentChild != m_childrenClocks.end(); ++currentChild )
		{
			if( ( *currentChild ) == childToRemove.lock() )
			{
				childToRemove.lock()->removeAllChildren();
				m_childrenClocks.erase( currentChild );
				break;
			}
		}
		return true;
	}

	bool Clock::removeAllChildren()
	{
		for( auto currentChild = m_childrenClocks.begin(); currentChild != m_childrenClocks.end(); ++currentChild )
		{
			(*currentChild)->removeAllChildren();
		}

		m_childrenClocks.clear();
		return true;
	}

	void Clock::togglePause()
	{
		m_isPaused = !m_isPaused;
	}

	void Clock::addAlarm( const std::string& eventName, const NamedProperties& eventArguments, double alarmTimeSeconds )
	{
		std::shared_ptr< Alarm > newAlarm = std::make_shared<Alarm>( m_currentTimeMicroseconds + microseconds( alarmTimeSeconds * secondsToMicroseconds ), eventArguments, eventName );

		m_alarms.push_back( newAlarm );
	}

	void Clock::checkAlarms()
	{
		for( auto currentAlarm = m_alarms.begin(); currentAlarm != m_alarms.end(); )
		{
			if( (*currentAlarm)->checkAlarm( m_currentTimeMicroseconds ) )
			{
				currentAlarm = m_alarms.erase( currentAlarm );
			}
			else
			{
				++currentAlarm;
			}
		}
	}

	void Clock::setMaximumDelta( double maximumDeltaTimeSeconds )
	{
		m_maximumDeltaMicroseconds = microseconds( maximumDeltaTimeSeconds * secondsToMicroseconds );
	}

	void Clock::setTimeScale( float timeScale )
	{
		timeScale = double( timeScale );
	}

	double Clock::getTimeSeconds()
	{
		return m_currentTimeMicroseconds  * microsecondsToSecondsDivision;
	}

	std::shared_ptr< Clock > Clock::s_instance;
}