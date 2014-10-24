#include "Light.h"
#include "MathUtilities.hpp"
#include "IncludeGL.h"

Light::Light()
	:	m_pointColor( Vector3() )
	,	m_spotColor( Vector3() )
	,	m_directionalColor( Vector3() )
	,	m_ambientColor( Vector3() )
	,	m_pointBrightness( 0.f )
	,	m_spotBrightness( 0.f )
	,	m_directionalBrightness( 0.f )
	,	m_ambientBrightness( 0.f )
	,	m_position( Vector3() )
	,	m_direction( Vector3() )
	,	m_innerRadius( 0.f )
	,	m_outerRadius( 1.f )
	,	m_innerDot( 0.f )
	,	m_outerDot( 1.f )
{}

void Light::render()
{
	glColor4f( m_renderColor.x, m_renderColor.y, m_renderColor.z, 1.f );
	glLineWidth( 2.f );
	glBegin( GL_LINES );

		glVertex3f( m_position.x + .125f, m_position.y, m_position.z );
		glVertex3f( m_position.x - .125f, m_position.y, m_position.z );

		glVertex3f( m_position.x, m_position.y + .125f, m_position.z );
		glVertex3f( m_position.x, m_position.y - .125f, m_position.z );

		glVertex3f( m_position.x, m_position.y, m_position.z + .125f );
		glVertex3f( m_position.x, m_position.y, m_position.z - .125f );

	glEnd();
	glLineWidth( 1.f );
	glColor4f( 1.f, 1.f, 1.f, 1.f );
}

void Light::makePointLight( const Vector3& lightPosition, const Vector3& pointColor, float innerRadius, float outerRadius, float lightPointBrightness )
{
	m_totalTime = 0.0;
	m_moving = false;
	m_position = lightPosition;
	m_stationaryPosition = m_position;
	m_pointColor = pointColor;
	m_renderColor = pointColor;

	if( outerRadius - innerRadius > 0.f )
	{
		m_outerRadius = outerRadius;
		m_innerRadius = innerRadius;
	}

	m_pointBrightness = lightPointBrightness;
}

void Light::makeDirectionalLight( const Vector3& direction, const Vector3& directionalColor, float intensity )
{
	m_direction = direction;
	m_directionalColor = directionalColor;
	m_renderColor = directionalColor;
	m_directionalBrightness = intensity;
}

float Light::getInnerRadius()
{
	return m_innerRadius;
}

float Light::getOuterRadius()
{
	return m_outerRadius;
}

float Light::getInnerDot()
{
	return m_innerDot;
}

float Light::getOuterDot()
{
	return m_outerDot;
}

void Light::makeSpotLight( const Vector3& lightPosition, const Vector3& spotLightColor, const Vector3& lightDirection, float innerRadius, float outerRadius, float innerAngleDegrees, float outerAngleDegrees, float spotLightBrightness )
{
	m_totalTime = 0.0;
	m_moving = false;
	m_position = lightPosition;
	m_stationaryPosition = m_position;
	m_direction = lightDirection;
	m_spotColor = spotLightColor;
	m_renderColor = spotLightColor;

	if( outerRadius - innerRadius > 0.f )
	{
		m_outerRadius = outerRadius;
		m_innerRadius = innerRadius;
	}

	float innerDot = cos( ConvertDegreesToRadians( innerAngleDegrees ) );
	float outerDot = cos( ConvertDegreesToRadians( outerAngleDegrees ) );

	if( innerDot - outerDot > 0.f )
	{
		m_innerDot = innerDot;
		m_outerDot = outerDot;
	}

	m_spotBrightness = spotLightBrightness;
}

void Light::update( double deltaTime )
{
	m_totalTime += deltaTime;

	if( m_moving )
	{
		//movement is hardcoded at the moment, it will later implement a strategy method
		m_position = Vector3( float( sin( m_totalTime ) ), float( cos( m_totalTime ) ) * 2.f, 1.5f );
	}
}

void Light::toggleMove()
{
	m_moving = !m_moving;

	if( !m_moving )
	{
		m_position = m_stationaryPosition;
	}
}

void Light::setDirection( Vector3 newDirection )
{
	m_direction = newDirection;
}

void Light::setPosition( Vector3 newPosition )
{
	m_position = newPosition;
}

void Light::reset()
{
	m_pointColor =  Vector3();
	m_spotColor = Vector3();
	m_directionalColor = Vector3();
	m_ambientColor = Vector3();
	m_pointBrightness = 0.f;
	m_spotBrightness = 0.f;
	m_directionalBrightness = 0.f;
	m_ambientBrightness = 0.f;
	m_position = Vector3();
	m_direction = Vector3();
	m_innerRadius = 0.f;
	m_outerRadius = 1.f;
	m_innerDot = 0.f;
	m_outerDot = 1.f;
}
