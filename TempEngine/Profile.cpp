#include <algorithm>

#include "Profile.h"
#include "ProfileClock.h"
#include "DebugUtility.h"
#include "Renderer.h"

//#include "MemoryUtilities.h"

namespace gh
{
	void Profile::createProfile( const std::string& profileName )
	{
		std::string profileLowercase = profileName;

		//see if the the clock has not been create previously
		std::transform( profileName.begin(), profileName.end(), profileLowercase.begin(), ::tolower );
		auto profileMappedLocation = s_stringToProfileLocation.find( profileLowercase );

		//if the profile is not on the map, then we add it
		if( profileMappedLocation == s_stringToProfileLocation.end() )
		{
			s_stringToProfileLocation.insert( std::pair< std::string, std::shared_ptr< Profile > >( profileLowercase, new Profile( profileName ) ) );
		}
	}

	Profile::Profile( const std::string& profileName )
		:	m_profileName( profileName )
		,	m_liveClockLocation( -1 )
		,	m_totalTime( 0 )
	{}

	std::weak_ptr< Profile > Profile::getProfile( const std::string& profileName )
	{
		std::string profileLowercase = profileName;

		//see if the the clock has not been create previously
		std::transform( profileName.begin(), profileName.end(), profileLowercase.begin(), ::tolower );
		auto profileMappedLocation = s_stringToProfileLocation.find( profileLowercase );

		//if the profile is not on the map, then we add it
		if( profileMappedLocation != s_stringToProfileLocation.end() )
		{
			return profileMappedLocation->second;
		}
		else
		{
			return std::weak_ptr< Profile >();
		}
	}

	void Profile::startProfiling( const std::string& profileSectionName, unsigned long long currentFrame )
	{
		std::string profileSectionLowercase = profileSectionName;
		
		std::transform( profileSectionName.begin(), profileSectionName.end(), profileSectionLowercase.begin(), ::tolower);
		auto profileSectionLocation = m_profileSectionsLocations.find( profileSectionLowercase );
		std::weak_ptr< ProfileClock > currentProfileSection;

		//if the profile section is not on the map, then we add it
		if( profileSectionLocation == m_profileSectionsLocations.end() )
		{
			m_profileSectionsLocations.insert( std::pair< std::string, int >( profileSectionLowercase, m_profilingSections.size() ) );
			m_profilingSections.push_back( std::make_shared< ProfileClock >( shared_from_this() ) );
			m_profilingSections.back()->setSectionName( profileSectionName );
			currentProfileSection = ( m_profilingSections.back() );
		}
		else
		{
			currentProfileSection = ( m_profilingSections[ profileSectionLocation->second ] );
		}

		m_totalTime += currentProfileSection.lock()->startProfiling( currentFrame );
	}

	void Profile::stopProfiling( const std::string& profileSectionName )
	{
		std::string profileSectionLowercase = profileSectionName;

		std::transform( profileSectionName.begin(), profileSectionName.end(), profileSectionLowercase.begin(), ::tolower);
		auto profileSectionLocation = m_profileSectionsLocations.find( profileSectionLowercase );
		std::weak_ptr< ProfileClock > currentProfileSection;

		//if the profile section is not on the map, then we don't do anything
		if( profileSectionLocation == m_profileSectionsLocations.end() )
		{
			return;
		}
		else
		{
			currentProfileSection = m_profilingSections[ profileSectionLocation->second ];
			m_totalTime += currentProfileSection.lock()->stopProfiling();
		}
	}

	void Profile::renderProfiles()
	{
		//debug
		for( auto currentProfile = s_liveProfileClocks.begin(); currentProfile != s_liveProfileClocks.end(); ++currentProfile )
		{
			//traceString( std::to_string( long double( ClocksToSeconds( (*currentProfile).lock()->m_totalTime ) ) ) + "\t" );

			(*currentProfile).lock()->renderProfileStatistics();
		}
	}

	void Profile::renderProfileStatistics()
	{
		AABB2 renderWindow = m_renderWindow;
		
		theRenderer.drawText( m_profileName.c_str(), "test", m_fontSize, renderWindow, BOX_ALIGN | TEXT_ALIGN_CENTERX | TEXT_ALIGN_TOP, toRGBA( 1.f, 0.f, 0.f ) );
		renderWindow.setMaxs( renderWindow.getMaxs() - Vector2( 0.f, m_fontSize ) );
		theRenderer.drawText( ( "Total Time = " + std::to_string( long double( ClocksToSeconds( m_totalTime ) ) ) ).c_str(), "test", m_fontSize, renderWindow, BOX_ALIGN | TEXT_ALIGN_CENTERX | TEXT_ALIGN_TOP, toRGBA( 1.f, 1.f, 1.f ) );
		renderWindow.setMaxs( renderWindow.getMaxs() - Vector2( 0.f, m_fontSize ) );

		for( auto currentProfileClock = m_profilingSections.begin(); currentProfileClock != m_profilingSections.end(); ++currentProfileClock )
		{
			(*currentProfileClock)->renderProfileStatistics( renderWindow, m_fontSize );
		}
	}

	void Profile::showProfile( const AABB2& renderWindow, float fontSize )
	{
		m_renderWindow = renderWindow;
		m_fontSize = fontSize;

		if( m_liveClockLocation == -1 )
		{
			m_liveClockLocation = s_liveProfileClocks.size();
			s_liveProfileClocks.push_back( shared_from_this() );
		}
	}

	void Profile::showProfile( const std::string& profileName, const AABB2& renderWindow, float fontSize )
	{
		std::string profileLowercase = profileName;

		//see if the the clock has not been create previously
		std::transform( profileName.begin(), profileName.end(), profileLowercase.begin(), ::tolower );
		auto profileMappedLocation = s_stringToProfileLocation.find( profileLowercase );

		//if the profile is not on the map, then we add it
		if( profileMappedLocation != s_stringToProfileLocation.end() )
		{
			profileMappedLocation->second->showProfile( renderWindow, fontSize );
		}
	}

	void Profile::hideProfile()
	{
		if( m_liveClockLocation != -1 )
		{
			s_liveProfileClocks.erase( s_liveProfileClocks.begin() + m_liveClockLocation );

			int liveClockIndex = 0;
			for( auto currentLiveClock = s_liveProfileClocks.begin(); currentLiveClock != s_liveProfileClocks.end(); ++currentLiveClock )
			{
				currentLiveClock->lock()->m_liveClockLocation = liveClockIndex;
				++liveClockIndex;
			}

			m_liveClockLocation = -1;
		}
	}

	void Profile::hideProfile( const std::string& profileName )
	{
		std::string profileLowercase = profileName;

		//see if the the clock has not been create previously
		std::transform( profileName.begin(), profileName.end(), profileLowercase.begin(), ::tolower );
		auto profileMappedLocation = s_stringToProfileLocation.find( profileLowercase );

		//if the profile is not on the map, then we add it
		if( profileMappedLocation != s_stringToProfileLocation.end() )
		{
			profileMappedLocation->second->hideProfile();
		}
	}

	double Profile::getTime()
	{
		return ClocksToSeconds(m_totalTime);
	}

	std::map< std::string, std::shared_ptr< Profile > > Profile::s_stringToProfileLocation;

	std::vector< std::weak_ptr< Profile > > Profile::s_liveProfileClocks;

}