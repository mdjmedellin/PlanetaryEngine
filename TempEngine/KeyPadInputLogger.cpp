#include "KeyPadInputLogger.h"
#include <xutility>

namespace gh
{
	KeyPadInputLogger::KeyPadInputLogger()
	{
		std::fill_n( m_keyDown, 256, false );
	}

	bool KeyPadInputLogger::isKeyPressed( const char& key ) const
	{
		return m_keyDown[ key ];
	}

	void KeyPadInputLogger::onKeyDown( const char& key )
	{
		m_keyDown[ key ] = true;
	}

	void KeyPadInputLogger::onKeyUp( const char& key )
	{
		m_keyDown[ key ] = false;
	}

}