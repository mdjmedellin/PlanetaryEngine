#include "Alarm.h"
#include "EventSystem.h"

namespace gh
{
	Alarm::Alarm( unsigned long long alarmTimeMicroseconds, const NamedProperties& alarmEventArguments, const std::string& alarmEventName )
		:	m_alarmTimeMicroseconds( alarmTimeMicroseconds )
		,	m_alarmEventArguments( alarmEventArguments )
		,	m_alarmEventName( alarmEventName )
	{}

	//returns true if the alarm went off
	bool Alarm::checkAlarm( unsigned long long currentTimeMicroseconds )
	{
		if( currentTimeMicroseconds >= m_alarmTimeMicroseconds )
		{
			//trigger the alarm
			theEventSystem.FireEvent( m_alarmEventName, m_alarmEventArguments );
			return true;
		}
		return false;
	}
}