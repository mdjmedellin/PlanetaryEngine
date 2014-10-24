#pragma once
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Texture.h"
#include "IncludeGL.h"
#include "Matrix4X4.h"
#include "Vector3.hpp"
#include "Vector4.h"
#include <memory>

enum MATERIAL_TEXTURE{ DIFFUSE = 0,
 NORMAL,
 SPECULAR,
 EMMISIVE,
 BUMP,
 NOISE,
 COLOR,
 NUM_MATERIALS };

enum VERTEX_ATTRIBUTES{
	V_POSITION = 2,
	V_TEXCOORD = 4,
	V_COLOR = 8,
	V_NORMAL = 16,
	V_TANGENT = 32,
	V_BITANGENT = 64
};

namespace gh
{
	class ShaderProgram
	{
	public:
		unsigned programNumber();
		virtual void getAttributeLocations();
		virtual void getUniformLocations();
		void setUniforms( int genericDebugInt, float genericDebugFloat, double currentTime, const Matrix4X4& mvpMatrix, 
							const Vector3& cameraWorldPosition, float fogIntensity, float fogStart, float fogEnd, const Vector4& fogColor,
							float surfaceShininess, float useDiffuse, float useSurfaceColor, float useNormalMap, float useEmissive,
							float useSpecularMap, float useBumpMap, float useLight );
		void setLightUniforms();
		void setAttributeData( unsigned int vertexAttributes );
		void disableAttributes( unsigned int vertexAttributes );
		void setTexture( MATERIAL_TEXTURE textureType, std::shared_ptr< Texture > texturePtr );
		void unbindTexture( MATERIAL_TEXTURE textureType );
		
		GLuint m_programNumber;
		GLint m_debugIntLocation;
		GLint m_debugFloatLocation;
		GLint m_timeUniformLocation;
		GLint m_mvpMatrixLocation;
		GLint m_diffuseTexUniformLocation;
		GLint m_normalTexUniformLocation;
		GLint m_specularTexUniformLocation;
		GLint m_emissiveTexUniformLocation;
		GLint m_bumpTexUniformLocation;
		GLint m_positionAttribute;
		GLint m_texCoordAttribute;
		GLint m_colorAttribute;
		GLint m_normalAttribute;
		GLint m_tangentAttribute;
		GLint m_bitangentAttribute;
		GLint m_cameraWPosUniformLocation;
		GLint m_useFog;
		GLint m_fogStartUniformLocation;
		GLint m_fogEndUniformLocation;
		GLint m_fogColorUniformLocation;
		GLint m_shininessUniformLocation;
		GLint m_useDiffuseLocation;
		GLint m_useSurfaceColor;
		GLint m_useNormalMap;
		GLint m_useEmissive;
		GLint m_useSpecularMap;
		GLint m_useBumpMap;
		GLint m_noiseTexUniformLocation;
		GLint m_lightsLocation;
		GLint m_lightsPointColor;
		GLint m_lightsSpotColor;
		GLint m_lightsDirection;
		GLint m_lightsSpotBrightness;
		GLint m_lightsPointBrightness;
		GLint m_lightsInnerRadii;
		GLint m_lightsOuterRadii;
		GLint m_lightsInnerDots;
		GLint m_lightsOuterDots;
		GLint m_numberOfLights;
		GLint m_useLightUniformLocation;
		GLint m_lightsDirectionalBrightness;
		GLint m_lightsDirectionalColor;
		GLint m_colorTexUniformLocation;

		ShaderProgram( GLuint programNumber = 0 );
	};
}
#endif