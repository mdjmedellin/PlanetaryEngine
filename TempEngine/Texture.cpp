#include "Texture.h"
#include "FileManager.h"
#include <cassert>
#include <string>

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

#include <glew.h>

namespace gh
{
	Texture::Texture( const char* szFilePath )
		:	m_width( 0 )
		,	m_height( 0 )
		,	m_iD( 0 )
		,	m_radius( 0 )
	{
		if (szFilePath )
		{
			load( szFilePath );
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures( 1, &m_iD );
	}

	void Texture::load( const char* szFilePath )
	{
		// loads the texture at szFilePath, puts it into an OpenGl texture, and returns
		// the ID of the texture

		// Load file pixels into memory (RGBA in Byte order)
		//
		int nChannels;
		char* byteBuffer = nullptr;
		int bufferSize = 0;
		FileManager::loadFile( szFilePath, byteBuffer, bufferSize );
		unsigned char *pixels = stbi_load_from_memory( reinterpret_cast< unsigned char* >( byteBuffer ), bufferSize, &m_width, &m_height, &nChannels, 4);

		if( !pixels )
		{ 
			//reportError( std::string( "Error Loading File: '" ) + szFilePath + "'." );
		}

		// Load the memory pixels into OPENGL
		//
		glGenTextures( 1, &m_iD );
		glBindTexture( GL_TEXTURE_2D, m_iD );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		//super slow function don't call it again and again
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

		//glGenerateMipmap( GL_TEXTURE_2D );

		stbi_image_free( pixels );

		m_radius = ( m_height > m_width ? m_height : m_width ) * .5f;
	}

	void Texture::apply() const
	{
		assert(m_iD);
		
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_iD);
	}

	void Texture::applyNullTexture()
	{
		glBindTexture( GL_TEXTURE_2D, 0);
	}

	//width and height
	int Texture::width() const 
	{
		return m_width;
	}

	int Texture::height() const
	{
		return m_height;
	}

	float Texture::radius() const
	{
		return m_radius;
	}

	unsigned Texture::getID()
	{
		return m_iD;
	}

}