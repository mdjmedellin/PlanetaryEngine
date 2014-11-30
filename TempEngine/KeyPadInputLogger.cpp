#include "KeyPadInputLogger.h"
#include <xutility>

namespace gh
{
	KeyPadInputLogger::KeyPadInputLogger()
		:	m_characterJustPressed(-1)
		,	m_lastCharacterDown(-1)
	{
		std::fill_n( m_keyDown, 256, false );
		std::fill_n( m_previousKeyStates, 256, false);
	}

	bool KeyPadInputLogger::IsKeyPressed( const char& key ) const
	{
		return m_keyDown[ key ];
	}

	void KeyPadInputLogger::onKeyDown( const char& key )
	{
		m_keyDown[ key ] = true;

		if( !m_previousKeyStates[ key ] )
		{
			m_characterJustPressed = key;
			m_lastCharacterDown = key;
		}
	}

	void KeyPadInputLogger::onKeyUp( const char& key )
	{
		m_keyDown[ key ] = false;
		if(m_lastCharacterDown == key)
		{
			m_lastCharacterDown = -1;
		}
	}

	void KeyPadInputLogger::UpdateKeyStates()
	{
		static bool wasDown = false;
		//copy the new key states to the previous key states and reset the current key states
		memcpy(m_previousKeyStates, m_keyDown, sizeof(bool) * 256);

		//reset the character that was just pressed
		m_characterJustPressed = -1;
	}

	bool KeyPadInputLogger::WasPreviouslyDown( const char& key ) const
	{
		return m_previousKeyStates[key];
	}

	bool KeyPadInputLogger::WasJustPressed( const char& key ) const
	{
		if( m_keyDown[key] 
			&& !m_previousKeyStates[key] )
			{
				return true;
			}

			return false;
	}

	bool KeyPadInputLogger::CharacterWasJustPressed() const
	{
		if(m_characterJustPressed != -1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool KeyPadInputLogger::CharacterIsDown() const
	{
		return m_lastCharacterDown != -1;
	}

	char KeyPadInputLogger::GetLastPressedChar() const
	{
		if( m_lastCharacterDown != -1 )
		{
			return m_lastCharacterDown;
		}
		else
		{
			return 0;
		}
	}

	char KeyPadInputLogger::GetCharJustPressed() const
	{
		if( m_characterJustPressed != -1 )
		{
			return m_characterJustPressed;
		}
		else
		{
			return 0;
		}
	}

	bool KeyPadInputLogger::IsCharacterDown( const char& key ) const
	{
		return ( m_previousKeyStates[key] || m_keyDown[key] );
	}

}