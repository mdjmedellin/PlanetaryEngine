#pragma once
#ifndef ALARM_H
#define ALARM_H

//includes
#include "NamedProperties.h"
//

namespace gh
{
	class Alarm
	{
	public:
		Alarm( unsigned long long alarmTimeMicroseconds, const NamedProperties& alarmEventArguments,  const std::string& alarmEventName );
		bool checkAlarm( unsigned long long currentTimeMicroseconds );

	private:
		NamedProperties m_alarmEventArguments;
		std::string m_alarmEventName;
		unsigned long long m_alarmTimeMicroseconds;
	};
}

#endif