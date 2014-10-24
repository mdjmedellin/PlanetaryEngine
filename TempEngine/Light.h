#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "Vector3.hpp"
#include "Matrix4X4.h"

class Light
{
public:
	Light();
	void setPosition( Vector3 newPosition );
	void setDirection( Vector3 newDirection );
	void toggleMove();
	void update( double deltaTime );
	void makePointLight( const Vector3& lightPosition, const Vector3& pointColor, float innerRadius, float outerRadius, float lightPointBrightness );
	void makeSpotLight( const Vector3& lightPosition, const Vector3& spotLightColor, const Vector3& lightDirection, float innerRadius, float outerRadius, float innerAngleDegrees, float outerAngleDegrees, float spotLightBrightness );
	void makeDirectionalLight( const Vector3& direction, const Vector3& directionalColor, float intensity );
	void render();
	void reset();
	float getInnerRadius();
	float getOuterRadius();
	float getInnerDot();
	float getOuterDot();

	Vector3 m_pointColor;
	Vector3 m_spotColor;
	Vector3 m_directionalColor;
	Vector3 m_ambientColor;

	float m_pointBrightness;
	float m_spotBrightness;
	float m_directionalBrightness;
	float m_ambientBrightness;

	Vector3 m_position;
	Vector3 m_direction;

private:

	Vector3 m_renderColor;

	double m_totalTime;
	bool m_moving;
	Vector3 m_stationaryPosition;

	float m_innerRadius;
	float m_outerRadius;

	float m_innerDot;
	float m_outerDot;
};

#endif