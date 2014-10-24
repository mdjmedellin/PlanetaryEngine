#pragma once
#ifndef PROFILE_H
#define PROFILE_H

//includes
#include "TimeUtility.h"
#include "AABB2.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
//

namespace gh
{
	class ProfileClock;

	class Profile : public std::enable_shared_from_this< Profile >
	{
	public:
		static void showProfile( const std::string& profileName, const AABB2& renderWindow, float fontSize );
		static void hideProfile( const std::string& profileName );
		static void renderProfiles();
		static void createProfile( const std::string& profileName );
		static std::weak_ptr< Profile > getProfile( const std::string& profileName );

		void startProfiling( const std::string& profileSectionName, unsigned long long currentFrame );
		void stopProfiling( const std::string& profileSectionName );
		void showProfile( const AABB2& renderWindow, float fontSize );
		void hideProfile();
		double getTime();

	private:
		std::string m_profileName;
		int m_liveClockLocation;
		SystemClocks m_totalTime;
		AABB2 m_renderWindow;
		float m_fontSize;
		std::vector< std::shared_ptr< ProfileClock > > m_profilingSections;
		std::map< std::string, int > m_profileSectionsLocations;

		Profile( const std::string& profileName );
		Profile( const Profile& other ); // non construction-copyable
		Profile& operator=( const Profile& ); // non copyable
		void renderProfileStatistics();

		static std::vector< std::weak_ptr< Profile > > s_liveProfileClocks;
		static std::map< std::string, std::shared_ptr< Profile > > s_stringToProfileLocation;
	};
}

#endif