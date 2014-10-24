#include "Rgba.h"
#include "MathUtilities.hpp"
#include "Vector4.h"

namespace gh
{
	Rgba::Rgba( unsigned int rgbaValue /*= WHITE */ )
		:	m_RGBA( rgbaValue )
	{
		//deduce the red, green, blue and alpha values
		unsigned int bitmask =  255;
		float scale = 1.f / 255.f;

		//red
		m_R = ( rgbaValue & bitmask ) * scale;

		//green
		bitmask = bitmask << 8;
		m_G = ( ( rgbaValue & bitmask ) >> 8 ) * scale;

		//blue
		bitmask = bitmask << 8;
		m_B = ( ( rgbaValue & bitmask ) >> 16 ) * scale;

		//alpha
		bitmask = bitmask << 8;
		m_A = ( ( rgbaValue & bitmask ) >> 24 ) * scale;
	}

	Rgba::Rgba( float red, float green, float blue, float alpha /*= 1.f */ )
		:	m_R( ClampFloatWithinRange( 0.f, 1.f, red ) )
		,	m_G( ClampFloatWithinRange( 0.f, 1.f, green ) )
		,	m_B( ClampFloatWithinRange( 0.f, 1.f, blue ) )
		,	m_A( ClampFloatWithinRange( 0.f, 1.f, alpha ) )
	{
		m_RGBA = toRGBA( m_R, m_G, m_B, m_A );
	}

	Vector4 Rgba::getVector4() const
	{
		return Vector4( m_R, m_G, m_B, m_A );
	}

	unsigned int Rgba::BLACK = toRGBA( 0.f, 0.f, 0.f );

	unsigned int Rgba::GREEN = toRGBA( 0.f, 1.f, 0.f );

	unsigned int Rgba::BLUE = toRGBA( 0.f, 0.f, 1.f );

	unsigned int Rgba::RED = toRGBA( 1.f, 0.f, 0.f );

	unsigned int Rgba::WHITE = toRGBA( 1.f, 1.f, 1.f );

}