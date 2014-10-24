#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#ifndef TEXT_ALIGNMENT_FONT
#define TEXT_ALIGNMENT_FONT
#define TEXT_ALIGN_RIGHT	2
#define TEXT_ALIGN_LEFT		4
#define TEXT_ALIGN_TOP		8
#define TEXT_ALIGN_BOTTOM	16
#define TEXT_ALIGN_CENTERX	32
#define TEXT_ALIGN_CENTERY	64
#define BOX_ALIGN			128
#endif

#ifndef BUFFER_CLEAR_VARIABLES
#define BUFFER_CLEAR_VARIABLES
#define CLEAR_COLOR			GL_COLOR_BUFFER_BIT
#define CLEAR_DEPTH			GL_DEPTH_BUFFER_BIT
#endif

#include <map>
#include "BitmapFont.h"
#include "Texture.h"
#include "AABB2.hpp"
#include "Matrix4X4.h"

struct Vertex2PTC
{
	float x, y;
	float u, v;
	unsigned int RGBA;
};

struct CommandConsoleText
{
	unsigned int m_textTint;
	std::string m_textLine;

	CommandConsoleText( const std::string& textLine, unsigned int textTint )
		:	m_textLine( textLine )
		,	m_textTint( textTint )
	{}
};

namespace gh
{
	class FrameBufferObjects;
	class MatrixStack;
	class FramebufferShader;
	class ShaderProgram;

	typedef std::map< std::string, bool >::iterator loadedFontsIterator;
	typedef std::map< std::string, std::shared_ptr< BitmapFont > >::iterator bitmapFontsIterator;
	
	class Renderer
	{
	public:
		void disableAllTextures();
		void updateWindowSize( const Vector2& windowSize );
		void generateFramebufferObject( unsigned& framebufferObjectID, unsigned framebufferColorTexture, unsigned framebufferDepthTexture );
		void useRegularFBO( const Vector4& clearColor, unsigned long clearBits );
		void overwriteVBO( unsigned int& idVBO, size_t sizeOfDataBytes, const void* data );
		unsigned int generateIBO( size_t sizeOfDataBytes, const void* data );
		void bindVBO( unsigned int idVBO );
		void drawTriangles( unsigned int idIBO, unsigned int numIndices );
		void renderQuad( const AABB2& windowSize, const Vector4& tint, std::shared_ptr< Texture >backgroundTexture );
		std::shared_ptr< BitmapFont > loadFont( const char* fontDefinitionXML, const std::string& fontName, const std::vector< std::string >& glyphSheetsLocations );
		void drawText( const char* outputString, const std::string& fontName, float pixelHeight, const AABB2& renderQuad, unsigned char alignment = BOX_ALIGN, unsigned int textTint = 0x0 );
		std::shared_ptr< BitmapFont > getFont( const std::string& fontName );
		void renderDataArrayV2PTC( const std::vector< Vertex2PTC >&m_textDataArray, const std::string& fontName  );
		void clearText();
		std::vector< Vertex2PTC > generateTextDataArray( std::vector< CommandConsoleText > textLines, std::string fontName, float fontHeight, const AABB2& renderQuad, unsigned char alignment = TEXT_ALIGN_TOP, int renderDirection = -1 );
		Vector2 getWindowSize();
	
	private:
		void pushCharacterTextData( const Vector2& bottomLeft, const Vector2&topRight, const Vector2& minTexCoords, const Vector2& maxTexCoords, unsigned int textTint, std::vector< Vertex2PTC >&textDataArray );
		Vector2 as_Vec2( const char* vector2Values );
		void alignText( Vector2& textLocation, const AABB2& renderQuad, unsigned int alignment, const char* outputString, float pixelHeight, const std::shared_ptr<BitmapFont>& renderFont );

		std::map< std::string, std::shared_ptr< BitmapFont > >m_bitmapFonts;
		std::map< std::string, bool > m_loadedFonts;
		std::vector< Vertex2PTC >m_textDataArray;
		std::vector< std::shared_ptr< FrameBufferObjects > >m_bufferObjects;
		Vector2 m_windowSize;
		Vector2 m_depthDistance;
	};
}

extern gh::Renderer theRenderer;

#endif