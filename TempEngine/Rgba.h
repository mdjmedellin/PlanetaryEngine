#pragma once
#ifndef RGBA_H
#define RGBA_H


//Forward declarations
class Vector4;
//

namespace gh
{
	class Rgba
	{
	public:
		static unsigned int WHITE;
		static unsigned int RED;
		static unsigned int BLUE;
		static unsigned int GREEN;
		static unsigned int BLACK;

		Rgba( unsigned int rgbaValue = WHITE );
		Rgba( float red, float green, float blue, float alpha = 1.f );
		Vector4 getVector4() const;

		float m_R;
		float m_G;
		float m_B;
		float m_A;
		unsigned int m_RGBA;

	private:
		/*float m_R;
		float m_G;
		float m_B;
		float m_A;*/

		//unsigned int m_RGBA;
	};
}

#endif