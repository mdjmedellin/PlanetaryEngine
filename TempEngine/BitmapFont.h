#pragma once
#ifndef BITMAPFONT_H
#define BITMAPFONT_H

#include "Vector2.hpp"
#include <vector>
#include "Texture.h"

namespace gh
{
	struct CharData
	{
		Vector2 texCoordMins;
		Vector2 texCoordMaxs;
		float A;
		float B;
		float C;
		int glyphSheetNum;

		float calculatePixelWidth( float height )
		{
			return ( A + B + C ) * height;
		}
	};

	class BitmapFont
	{
	public:
		BitmapFont();
		BitmapFont( const std::vector< std::shared_ptr< Texture > >& glyphSheets, const std::vector< CharData >& fontCharacters, float pixelPerUnit );
		CharData getCharData( int index ) const;
		float calcTextPixelWidth( const char* outputString, float pixelHeight ) const;
		void applyTexture() const;

	private:
		float cellPixelHeight;
		std::vector< std::shared_ptr< Texture > > m_glyphSheets;
		std::vector< CharData > m_characters;
	};
}

#endif