#include "BitmapFont.h"
#include "pugixml.hpp"
#include "Texture.h"

namespace gh
{
	BitmapFont::BitmapFont( const std::vector< std::shared_ptr< Texture > >& glyphSheets, const std::vector< CharData >& fontCharacters, float pixelsPerUnit )
		:	m_glyphSheets( glyphSheets )
		,	m_characters( fontCharacters )
		,	cellPixelHeight( pixelsPerUnit )
	{}

	BitmapFont::BitmapFont()
		:	cellPixelHeight( 0.f )
	{}

	CharData BitmapFont::getCharData( int index ) const
	{
		return m_characters[ index ];
	}

	void BitmapFont::applyTexture() const
	{
		m_glyphSheets.back()->apply();
	}

	float BitmapFont::calcTextPixelWidth( const char* outputString, float pixelHeight ) const
	{
		float totalWidth = 0.f;

		for( size_t index = 0; *( outputString + index ); ++index )
		{
			totalWidth += getCharData( *( outputString + index ) ).calculatePixelWidth( pixelHeight );
		}

		return totalWidth;
	}
}

