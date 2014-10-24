#pragma once
#ifndef PROFILE_CLOCK_H
#define PROFILE_CLOCK_H

//includes
#include "TimeUtility.h"
#include "AABB2.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
//

namespace gh
{
	class Profile;

	class ProfileClock
	{
	public:
		ProfileClock( std::weak_ptr< Profile > parentProfile );
		SystemClocks startProfiling( unsigned long long );
		SystemClocks stopProfiling();
		void render();
		void renderProfileStatistics( AABB2& renderWindow, float fontSize );
		void setSectionName( const std::string& sectionName );

	private:
		std::weak_ptr< Profile > m_parentProfile;
		SystemClocks m_startProfileTime;
		SystemClocks m_totalTime;
		unsigned long long m_framesUsed;
		unsigned long long m_lastFrameUpdated;
		AABB2 m_renderBox;
		std::string m_profileName;
		std::string m_sectionName;
		int m_liveClockLocation;

		//void render();
		void setProfileName( const std::string& profileName );

		static std::vector< std::weak_ptr< ProfileClock > > s_liveProfileClocks;
		static std::map< std::string, std::shared_ptr< ProfileClock > > s_stringToProfileLocation;
	};
}

#endif