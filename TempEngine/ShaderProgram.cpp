#include "ShaderProgram.h"
#include "Matrix4X4.h"
#include "MathUtilities.hpp"
#include "LightManager.h"
#include "Vertex.h"

namespace gh
{
	ShaderProgram::ShaderProgram( GLuint programNumber /*= 0*/ )
		:	m_programNumber( programNumber )
		,	m_debugIntLocation( 0 )
		,	m_debugFloatLocation( 0 )
		,	m_timeUniformLocation( 0 )
		,	m_mvpMatrixLocation( 0 )
		,	m_diffuseTexUniformLocation( 0 )
		,	m_normalTexUniformLocation( 0 )
		,	m_specularTexUniformLocation( 0 )
		,	m_emissiveTexUniformLocation( 0 )
		,	m_bumpTexUniformLocation( 0 )
		,	m_positionAttribute( 0 )
		,	m_texCoordAttribute( 0 )
		,	m_colorAttribute( 0 )
		,	m_normalAttribute( 0 )
		,	m_tangentAttribute( 0 )
		,	m_bitangentAttribute( 0 )
		,	m_cameraWPosUniformLocation( 0 )
		,	m_fogStartUniformLocation( 0 )
		,	m_fogEndUniformLocation( 0 )
		,	m_fogColorUniformLocation( 0 )
		,	m_shininessUniformLocation( 0 )
		,	m_useDiffuseLocation( 0 )
		,	m_useSurfaceColor( 0 )
		,	m_useNormalMap( 0 )
		,	m_useEmissive( 0 )
		,	m_useSpecularMap( 0 )
		,	m_useBumpMap( 0 )
		,	m_noiseTexUniformLocation( 0 )
		,	m_lightsLocation( 0 )
		,	m_lightsPointColor( 0 )
		,	m_lightsSpotColor( 0 )
		,	m_lightsDirection( 0 )
		,	m_lightsSpotBrightness( 0 )
		,	m_lightsPointBrightness( 0 )
		,	m_lightsInnerRadii( 0 )
		,	m_lightsOuterRadii( 0 )
		,	m_lightsInnerDots( 0 )
		,	m_lightsOuterDots( 0 )
		,	m_numberOfLights( 0 )
		,	m_useLightUniformLocation( 0 )
		,	m_lightsDirectionalBrightness( 0 )
		,	m_lightsDirectionalColor( 0 )
		,	m_colorTexUniformLocation( 0 )
	{}

	void ShaderProgram::getUniformLocations()
	{
		m_debugIntLocation = glGetUniformLocation( m_programNumber, "u_genericDebugInt" );
		m_debugFloatLocation = glGetUniformLocation( m_programNumber, "u_genericDebugFloat" );
		m_timeUniformLocation = glGetUniformLocation( m_programNumber, "u_currentTime" );
		m_diffuseTexUniformLocation = glGetUniformLocation( m_programNumber, "u_diffuseTexUniform" );
		m_normalTexUniformLocation = glGetUniformLocation( m_programNumber, "u_normalTexUniform" );
		m_specularTexUniformLocation = glGetUniformLocation( m_programNumber, "u_specularTexUniform" );
		m_emissiveTexUniformLocation = glGetUniformLocation( m_programNumber, "u_emissiveTexUniform" );
		m_noiseTexUniformLocation = glGetUniformLocation( m_programNumber, "u_noiseTexUniform" );
		m_bumpTexUniformLocation = glGetUniformLocation( m_programNumber, "u_bumpTexUniform" );
		m_fogColorUniformLocation = glGetUniformLocation( m_programNumber, "u_fogColor" );
		m_lightsLocation = glGetUniformLocation( m_programNumber, "u_lightsWorldPosition" );
		m_lightsPointColor = glGetUniformLocation( m_programNumber, "u_lightsPointColor" );
		m_cameraWPosUniformLocation = glGetUniformLocation( m_programNumber, "u_cameraWorldPosition" );
		m_fogStartUniformLocation = glGetUniformLocation( m_programNumber, "u_fogStart" );
		m_fogEndUniformLocation = glGetUniformLocation( m_programNumber, "u_fogEnd" );
		m_shininessUniformLocation = glGetUniformLocation( m_programNumber, "u_surfaceShininess" );
		m_lightsPointBrightness = glGetUniformLocation( m_programNumber, "u_lightsPointBrightness" );
		m_lightsSpotBrightness = glGetUniformLocation( m_programNumber, "u_lightsSpotBrightness" );
		m_lightsInnerRadii = glGetUniformLocation( m_programNumber, "u_lightsInnerRadii" );
		m_lightsOuterRadii = glGetUniformLocation( m_programNumber, "u_lightsOuterRadii" );
		m_numberOfLights = glGetUniformLocation( m_programNumber, "u_numberOfLights" );
		m_useDiffuseLocation = glGetUniformLocation( m_programNumber, "u_useDiffuse" );
		m_useSurfaceColor = glGetUniformLocation( m_programNumber, "u_useSurfaceColor" );
		m_useNormalMap = glGetUniformLocation( m_programNumber, "u_useNormalMap" );
		m_useEmissive = glGetUniformLocation( m_programNumber, "u_useEmissive" );
		m_useSpecularMap = glGetUniformLocation( m_programNumber, "u_useSpecularMap" );
		m_useBumpMap = glGetUniformLocation( m_programNumber, "u_useBumpMap" );
		m_useFog = glGetUniformLocation( m_programNumber, "u_useFog" );
		m_mvpMatrixLocation = glGetUniformLocation( m_programNumber, "u_mvpMatrix" );
		m_lightsSpotColor = glGetUniformLocation( m_programNumber, "u_lightsSpotColor" );
		m_lightsDirection = glGetUniformLocation( m_programNumber, "u_lightsDirection" );
		m_lightsInnerDots = glGetUniformLocation( m_programNumber, "u_lightsInnerDots" );
		m_lightsOuterDots = glGetUniformLocation( m_programNumber, "u_lightsOuterDots" );
		m_useLightUniformLocation = glGetUniformLocation( m_programNumber, "u_useLight");
		m_lightsDirectionalColor = glGetUniformLocation( m_programNumber, "u_lightsDirectionalColor" );
		m_lightsDirectionalBrightness = glGetUniformLocation( m_programNumber, "u_lightsDirectionalBrightness" );
		m_colorTexUniformLocation = glGetUniformLocation( m_programNumber, "u_colorTexUniform" );
	}

	void ShaderProgram::getAttributeLocations()
	{
		m_positionAttribute = glGetAttribLocation( m_programNumber, "a_position" );
		m_texCoordAttribute = glGetAttribLocation( m_programNumber, "a_texCoord" );
		m_colorAttribute = glGetAttribLocation( m_programNumber, "a_color" );
		m_normalAttribute = glGetAttribLocation( m_programNumber, "a_normal" );
		m_tangentAttribute = glGetAttribLocation( m_programNumber, "a_tangent" );
		m_bitangentAttribute = glGetAttribLocation( m_programNumber, "a_bitangent" );
	}

	void ShaderProgram::setUniforms( int genericDebugInt, float genericDebugFloat, double currentTime, const Matrix4X4& mvpMatrix, 
										const Vector3& cameraWorldPosition, float fogIntensity, float fogStart, float fogEnd, 
										const Vector4& fogColor, float surfaceShininess, float useDiffuse, float useSurfaceColor,
										float useNormalMap, float useEmissive, float useSpecularMap, float useBumpMap, float useLight )
	{
		glUniform1i( m_debugIntLocation, genericDebugInt );
		glUniform1f( m_debugFloatLocation, genericDebugFloat );
		glUniform1f( m_timeUniformLocation, float( currentTime ) );
		glUniform4fv( m_fogColorUniformLocation, 1, &( fogColor.x ) );
		glUniform3fv( m_cameraWPosUniformLocation, 1, &cameraWorldPosition.x );
		glUniform1f( m_fogStartUniformLocation, fogStart );
		glUniform1f( m_fogEndUniformLocation, fogEnd );
		glUniform1f( m_shininessUniformLocation, surfaceShininess );
		glUniform1f( m_useDiffuseLocation, useDiffuse );
		glUniform1f( m_useSurfaceColor, useSurfaceColor );
		glUniform1f( m_useNormalMap, useNormalMap );
		glUniform1f( m_useSpecularMap, useSpecularMap );
		glUniform1f( m_useBumpMap, useBumpMap );
		glUniform1f( m_useEmissive, useEmissive );
		glUniform1f( m_useFog, fogIntensity );
		glUniformMatrix4fv( m_mvpMatrixLocation, 1, GL_FALSE, &( mvpMatrix.m_values[ 0 ] ) );
		glUniform1f( m_useLightUniformLocation, useLight );

		//setLightUniforms();
	}
	
	void ShaderProgram::setAttributeData( unsigned int vertexAttributes )
	{
		if( vertexAttributes & V_POSITION )
		{
			glEnableVertexAttribArray( m_positionAttribute );
			glVertexAttribPointer( m_positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, x ) ) );
		}
		if( vertexAttributes & V_TEXCOORD )
		{
			glEnableVertexAttribArray( m_texCoordAttribute );
			glVertexAttribPointer( m_texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, u ) ) );
		}
		if( vertexAttributes & V_COLOR )
		{
			glEnableVertexAttribArray( m_colorAttribute );
			glVertexAttribPointer( m_colorAttribute, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, rgba ) ) );
		}
		if( vertexAttributes & V_NORMAL )
		{
			glEnableVertexAttribArray( m_normalAttribute );
			glVertexAttribPointer( m_normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, normal ) ) );
		}
		if( vertexAttributes & V_TANGENT )
		{
			glEnableVertexAttribArray( m_tangentAttribute );
			glVertexAttribPointer( m_tangentAttribute, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, tangent ) ) );
		}
		if( vertexAttributes & V_BITANGENT )
		{
			glEnableVertexAttribArray( m_bitangentAttribute );
			glVertexAttribPointer( m_bitangentAttribute, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
				reinterpret_cast< const GLvoid* >( offsetof( Vertex, bitangent ) ) );
		}
	}

	void ShaderProgram::disableAttributes( unsigned int vertexAttributes )
	{
		if( vertexAttributes & V_POSITION )
		{
			glDisableVertexAttribArray( m_positionAttribute );
		}
		if( vertexAttributes & V_TEXCOORD )
		{
			glDisableVertexAttribArray( m_texCoordAttribute );
		}
		if( vertexAttributes & V_COLOR )
		{
			glDisableVertexAttribArray( m_colorAttribute );
		}
		if( vertexAttributes & V_NORMAL )
		{
			glDisableVertexAttribArray( m_normalAttribute );
		}
		if( vertexAttributes & V_TANGENT )
		{
			glDisableVertexAttribArray( m_tangentAttribute );
		}
		if( vertexAttributes & V_BITANGENT )
		{
			glDisableVertexAttribArray( m_bitangentAttribute );
		}
	}

	unsigned ShaderProgram::programNumber()
	{
		return m_programNumber;
	}

	void ShaderProgram::setLightUniforms()
	{
		int lightsAvailable = LightManager::instance().lightsAvailable();
		glUniform3fv( m_lightsLocation, lightsAvailable, &LightManager::instance().getPositions()[0][0] );
		glUniform3fv( m_lightsPointColor, lightsAvailable, &LightManager::instance().getPointColor()[0][0] );
		glUniform3fv( m_lightsSpotColor, lightsAvailable, &LightManager::instance().getSpotColor()[0][0] );
		glUniform3fv( m_lightsDirection, lightsAvailable, &LightManager::instance().getDirections()[0][0] );
		glUniform3fv( m_lightsDirectionalColor, lightsAvailable, &LightManager::instance().getDirectionalColor()[0][0] );
		glUniform1fv( m_lightsDirectionalBrightness, lightsAvailable, &LightManager::instance().getDirectionalBrightness()[0] );
		glUniform1fv( m_lightsSpotBrightness, lightsAvailable, &LightManager::instance().getSpotBrightness()[0] );
		glUniform1fv( m_lightsPointBrightness, lightsAvailable, &LightManager::instance().getPointBrightness()[0] );
		glUniform1fv( m_lightsInnerRadii, lightsAvailable, &LightManager::instance().getInnerRadii()[0] );
		glUniform1fv( m_lightsOuterRadii, lightsAvailable, &LightManager::instance().getOuterRadii()[0] );
		glUniform1fv( m_lightsInnerDots, lightsAvailable, &LightManager::instance().getInnerDots()[0] );
		glUniform1fv( m_lightsOuterDots, lightsAvailable, &LightManager::instance().getOuterDots()[0] );
		glUniform1i( m_numberOfLights, lightsAvailable );
	}

	void ShaderProgram::setTexture( MATERIAL_TEXTURE textureType, std::shared_ptr< Texture > texturePtr )
	{
		switch( textureType )
		{
		case DIFFUSE:
			glActiveTexture( GL_TEXTURE0 );
			glEnable( GL_TEXTURE_2D );
			texturePtr->apply();
			glUniform1i( m_diffuseTexUniformLocation, 0 );
			break;
		case NORMAL:
			glActiveTexture( GL_TEXTURE1 );
			glEnable( GL_TEXTURE_2D );
			texturePtr->apply();
			glUniform1i( m_normalTexUniformLocation, 1 );
			break;
		case SPECULAR:
			glActiveTexture( GL_TEXTURE2 );
			glEnable( GL_TEXTURE_2D );
			texturePtr->apply();
			glUniform1i( m_specularTexUniformLocation, 2 );
			break;
		case EMMISIVE:
			glActiveTexture( GL_TEXTURE3 );
			glEnable( GL_TEXTURE_2D );
			texturePtr->apply();
			glUniform1i( m_emissiveTexUniformLocation, 3 );
			break;
		case BUMP:
			glActiveTexture( GL_TEXTURE4 );
			glEnable( GL_TEXTURE_2D );
			texturePtr->apply();
			glUniform1i( m_bumpTexUniformLocation, 4 );
			break;
		case NOISE:
			glActiveTexture( GL_TEXTURE5 );
			glEnable( GL_TEXTURE_2D );
			texturePtr->apply();
			glUniform1i( m_noiseTexUniformLocation, 5 );
			break;
		case COLOR:
			glActiveTexture( GL_TEXTURE6 );
			glEnable( GL_TEXTURE_2D );
			texturePtr->apply();
			glUniform1i( m_colorTexUniformLocation, 6 );
			break;
		default:
			break;
		}
	}

	void ShaderProgram::unbindTexture( MATERIAL_TEXTURE textureType )
	{
		switch( textureType )
		{
		case DIFFUSE:
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glDisable( GL_TEXTURE_2D );
			break;
		case NORMAL:
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glDisable( GL_TEXTURE_2D );
			break;
		case SPECULAR:
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glDisable( GL_TEXTURE_2D );
			break;
		case EMMISIVE:
			glActiveTexture( GL_TEXTURE3 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glDisable( GL_TEXTURE_2D );
			break;
		case BUMP:
			glActiveTexture( GL_TEXTURE4 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glDisable( GL_TEXTURE_2D );
			break;
		case NOISE:
			glActiveTexture( GL_TEXTURE5 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glDisable( GL_TEXTURE_2D );
			break;
		case COLOR:
			glActiveTexture( GL_TEXTURE6 );
			glBindTexture( GL_TEXTURE_2D, 0 );
			glDisable( GL_TEXTURE_2D );
			break;
		default:
			break;
		}
	}
}