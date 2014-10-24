#include "LightManager.h"
#include "Light.h"
#include <algorithm>
#include <functional>
#include <cassert>

void LightManager::addLight( std::shared_ptr< Light > newLight )
{
	m_lights.push_back( newLight );
}

void LightManager::create()
{
	assert( !s_instance );
	s_instance.reset( new LightManager );
}

void LightManager::destroy()
{
	assert( s_instance );
	s_instance->m_lights.clear();
	s_instance.reset();
}

LightManager& LightManager::instance()
{
	assert( s_instance );
	return *s_instance;
}

std::vector< std::shared_ptr< Light > > LightManager::getLights()
{
	return m_lights;
}

std::vector< Vector3 > LightManager::getPositions()
{
	std::vector< Vector3 > lightPositions;

	iterateLights( 
		[&]( std::shared_ptr< Light > currentLight )
		{
			lightPositions.push_back( currentLight->m_position );
		}
	);

	return lightPositions;
}

void LightManager::iterateLights( std::function< void ( std::shared_ptr< Light > currentLight ) > loopFunction )
{
	std::for_each( m_lights.begin(), m_lights.end(), [&]( std::shared_ptr< Light > currentLight )
	{
		loopFunction( currentLight );
	});
}

std::vector< Vector3 > LightManager::getPointColor()
{
	std::vector< Vector3 > lightPointColors;

	iterateLights( 
		[&]( std::shared_ptr< Light > currentLight )
	{
		lightPointColors.push_back( currentLight->m_pointColor );
	}
	);

	return lightPointColors;
}

std::vector< float > LightManager::getPointBrightness()
{
	std::vector< float > lightPointBrightness;

	iterateLights( 
		[&]( std::shared_ptr< Light > currentLight )
	{
		lightPointBrightness.push_back( currentLight->m_pointBrightness );
	}
	);

	return lightPointBrightness;
}

std::vector< float > LightManager::getInnerRadii()
{
	std::vector< float > lightInnerRadii;

	iterateLights( 
		[&]( std::shared_ptr< Light > currentLight )
	{
		lightInnerRadii.push_back( currentLight->getInnerRadius() );
	}
	);

	return lightInnerRadii;
}

std::vector< float > LightManager::getOuterRadii()
{
	std::vector< float > lightOuterRadii;

	iterateLights( 
		[&]( std::shared_ptr< Light > currentLight )
	{
		lightOuterRadii.push_back( currentLight->getOuterRadius() );
	}
	);

	return lightOuterRadii;
}

int LightManager::lightsAvailable()
{
	return m_lights.size();
}

std::vector< Vector3 > LightManager::getDirections()
{
	std::vector< Vector3 > lightsDirection;
	iterateLights( [&]( std::shared_ptr< Light > currentLight )
	{
		lightsDirection.push_back( currentLight->m_direction );
	}
	);

	return lightsDirection;
}

std::vector< Vector3 > LightManager::getSpotColor()
{
	std::vector< Vector3 > lightsSpotColor;
	iterateLights( [&]( std::shared_ptr< Light > currentLight )
	{
		lightsSpotColor.push_back( currentLight->m_spotColor );
	}
	);

	return lightsSpotColor;
}

std::vector< float > LightManager::getSpotBrightness()
{
	std::vector< float > lightsSpotBrightness;
	iterateLights( [&]( std::shared_ptr< Light > currentLight )
	{
		lightsSpotBrightness.push_back( currentLight->m_spotBrightness );
	}
	);

	return lightsSpotBrightness;
}

std::vector< float > LightManager::getInnerDots()
{
	std::vector< float > lightsInnerDots;
	iterateLights( [&]( std::shared_ptr< Light > currentLight )
	{
		lightsInnerDots.push_back( currentLight->getInnerDot() );
	}
	);

	return lightsInnerDots;
}

std::vector< float > LightManager::getOuterDots()
{
	std::vector< float > lightsOuterDots;
	iterateLights( [&]( std::shared_ptr< Light > currentLight )
	{
		lightsOuterDots.push_back( currentLight->getOuterDot() );
	}
	);

	return lightsOuterDots;
}

void LightManager::updateLights( double deltaTime)
{
	iterateLights( [&] ( std::shared_ptr< Light > currentLight )
	{
		currentLight->update( deltaTime );
	});
}

void LightManager::toggleLightsMove()
{
	iterateLights( [&] ( std::shared_ptr< Light > currentLight )
	{
		currentLight->toggleMove();
	});
}

void LightManager::renderLights()
{
	iterateLights( [&] ( std::shared_ptr< Light > currentLight )
	{
		currentLight->render();
	});
}

std::vector< Vector3 > LightManager::getDirectionalColor()
{
	std::vector< Vector3 > directionalColors;

	iterateLights( [&] ( std::shared_ptr< Light > currentLight )
	{
		directionalColors.push_back( currentLight->m_directionalColor );
	});

	return directionalColors;
}

std::vector< float > LightManager::getDirectionalBrightness()
{
	std::vector< float > directionalBrightness;

	iterateLights( [&] ( std::shared_ptr< Light > currentLight )
	{
		directionalBrightness.push_back( currentLight->m_directionalBrightness );
	});

	return directionalBrightness;
}

std::unique_ptr< LightManager > LightManager::s_instance;