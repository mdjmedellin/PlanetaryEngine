#include <cassert>
#include <string>
#include <glew.h>


#include "TextureManager.h"
#include "Texture.h"

//#include "MemoryUtilities.h"

namespace gh
{
	std::shared_ptr< Texture > TextureManager::getTexture( const char* szFilePath )
	{
		auto iter = m_pathsToTextures.find( szFilePath );

		if( iter != m_pathsToTextures.end() )
		{
			return iter->second;
		}
		else
		{
			auto pTexture = std::make_shared< Texture >( szFilePath );
			m_pathsToTextures[ szFilePath ] = pTexture;
			return pTexture;
		}
	}

	void TextureManager::create()
	{
		assert( !s_instance );
		s_instance.reset( new TextureManager );
	}

	void TextureManager::destroy()
	{
		assert( s_instance );
		s_instance->m_pathsToTextures.clear();
		s_instance.reset();
	}

	TextureManager& TextureManager::instance()
	{
		assert( s_instance );
		return *s_instance;
	}

	void TextureManager::generateFramebufferColorTexture( unsigned& framebufferColorTextureID, const Vector2& windowSize )
	{
		glActiveTexture( GL_TEXTURE0 );
		glGenTextures( 1, &framebufferColorTextureID );
		glBindTexture( GL_TEXTURE_2D, framebufferColorTextureID );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, (int)windowSize.x, (int)windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	}

	void TextureManager::generateFramebufferDepthTexture( unsigned& framebufferDepthTextureID, const Vector2& windowSize )
	{
		glActiveTexture( GL_TEXTURE1 );
		glGenTextures( 1, &framebufferDepthTextureID );
		glBindTexture( GL_TEXTURE_2D, framebufferDepthTextureID );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)windowSize.x, (int)windowSize.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL );
	}

	std::unique_ptr< TextureManager > TextureManager::s_instance;

}