#pragma once

namespace gh
{
	class Texture
	{
	public:
		explicit Texture( const char* szFilePath);
		~Texture();

		void load( const char* szFilePath );
		void apply() const;

		unsigned getID();

		static void applyNullTexture();

		//width and height
		int width()const;
		int height() const;
		float radius() const;

	private:
		float m_radius;
		int m_width, m_height;
		unsigned m_iD;
	};
}