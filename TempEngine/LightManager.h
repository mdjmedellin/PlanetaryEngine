#pragma once
#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include <functional>
#include <memory>
#include <vector>

class Vector3;
class Light;

class LightManager
{
public:
	void renderLights();
	void updateLights( double deltaTime );
	void toggleLightsMove();
	void setLightsStationary();
	void addLight( std::shared_ptr< Light > newLight );
	std::vector< std::shared_ptr< Light > > getLights();
	std::vector< Vector3 > getPositions();
	std::vector< Vector3 > getPointColor();
	std::vector< Vector3 > getSpotColor();
	std::vector< Vector3 > getDirectionalColor();
	std::vector< Vector3 > getDirections();
	std::vector< float > getSpotBrightness();
	std::vector< float > getPointBrightness();
	std::vector< float > getDirectionalBrightness();
	std::vector< float > getInnerRadii();
	std::vector< float > getOuterRadii();
	std::vector< float > getInnerDots();
	std::vector< float > getOuterDots();
	int lightsAvailable();

	static void create();
	static void destroy();
	static LightManager& instance();

protected:
	std::vector< std::shared_ptr< Light >  > m_lights;
	static std::unique_ptr< LightManager > s_instance;

private:
	void iterateLights( std::function< void ( std::shared_ptr< Light > currentLight ) > loopFunction );
};

#endif