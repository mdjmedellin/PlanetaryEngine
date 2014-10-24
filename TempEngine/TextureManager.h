#pragma once

//includes
#include <memory>
#include <map>
#include "Vector2.hpp"
//

namespace gh
{
	class Texture;

	class TextureManager
	{
	public:

		std::shared_ptr< Texture > getTexture( const char* szFilePath );

		void generateFramebufferColorTexture( unsigned& framebufferColorTextureID, const Vector2& windowSize );

		void generateFramebufferDepthTexture( unsigned& framebufferDepthTextureID, const Vector2& windowSize );

		static void create();

		static void destroy();

		static TextureManager& instance();

	protected:
		std::map< std::string, std::shared_ptr< Texture > > m_pathsToTextures;
		static std::unique_ptr< TextureManager > s_instance;
	};
}

