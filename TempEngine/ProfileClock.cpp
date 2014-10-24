#include "ProfileClock.h"
#include "Profile.h"
#include "Renderer.h"
#include "DebugUtility.h"
#include <algorithm>

namespace gh
{
	SystemClocks ProfileClock::startProfiling( unsigned long long currentFrame )
	{
		if( m_lastFrameUpdated != currentFrame )
		{
			m_lastFrameUpdated = currentFrame;
			++m_framesUsed;
			m_startProfileTime = GetAbsoluteTimeClocks();
			return 0;
		}
		else
		{
			return stopProfiling();
		}
	}

	SystemClocks ProfileClock::stopProfiling()
	{
		SystemClocks endTime = GetAbsoluteTimeClocks();
		SystemClocks timeDifference = endTime - m_startProfileTime;
		m_totalTime += timeDifference;
		m_startProfileTime = endTime;
		return timeDifference;
	}

	ProfileClock::ProfileClock( std::weak_ptr< Profile > parentProfile )
		:	m_startProfileTime( 0 )
		,	m_totalTime( 0 )
		,	m_lastFrameUpdated( 0 )
		,	m_framesUsed( 0 )
		,	m_liveClockLocation( -1 )
		,	m_profileName("")
		,	m_parentProfile( parentProfile )
	{}

	void ProfileClock::renderProfileStatistics( AABB2& renderWindow, float fontSize )
	{
		AABB2 renderQuad = renderWindow;
		renderQuad.setMins( renderQuad.getMaxs() - Vector2( renderQuad.getMaxs().x, fontSize ) );
		theRenderer.drawText( m_sectionName.c_str(), "test", fontSize, renderQuad, BOX_ALIGN | TEXT_ALIGN_CENTERX | TEXT_ALIGN_BOTTOM, toRGBA( 0.f, 1.f, 0.f ) );
		renderQuad.setMins( renderQuad.getMins() - Vector2( 0.f, fontSize ) );
		theRenderer.drawText( ( "Total Time = " + std::to_string( long double( gh::ClocksToSeconds( m_totalTime ) ) ) ).c_str(), "test", fontSize, renderQuad, BOX_ALIGN | TEXT_ALIGN_CENTERX | TEXT_ALIGN_BOTTOM, toRGBA( 1.f, 1.f, 1.f ) );
		renderQuad.setMins( renderQuad.getMins() - Vector2( 0.f, fontSize ) );
		theRenderer.drawText( ( "Average Time = " + std::to_string( long double( gh::ClocksToSeconds( m_totalTime ) / m_framesUsed ) ) ).c_str(), "test", fontSize, renderQuad, BOX_ALIGN | TEXT_ALIGN_CENTERX | TEXT_ALIGN_BOTTOM, toRGBA( 1.f, 1.f, 1.f ) );
		renderQuad.setMins( renderQuad.getMins() - Vector2( 0.f, fontSize ) );
		theRenderer.drawText( ( "Percent of Time Spent = " + std::to_string( long long ( gh::ClocksToSeconds( m_totalTime ) * 100.0 / m_parentProfile.lock()->getTime() ) ) ).c_str(), "test", fontSize, renderQuad, BOX_ALIGN | TEXT_ALIGN_CENTERX | TEXT_ALIGN_BOTTOM, toRGBA( 1.f, 1.f, 1.f ) );
		renderWindow.setMaxs( renderWindow.getMaxs() - Vector2( 0.f, fontSize * 4.f ) ) ;
	}

	void ProfileClock::setProfileName( const std::string& profileName )
	{
		m_profileName = profileName;
	}

	void ProfileClock::setSectionName( const std::string& sectionName )
	{
		m_sectionName = sectionName;
	}

	std::vector< std::weak_ptr< ProfileClock > > ProfileClock::s_liveProfileClocks;
	std::map< std::string, std::shared_ptr< ProfileClock > > ProfileClock::s_stringToProfileLocation;

}