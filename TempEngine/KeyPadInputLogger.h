#ifndef KEYPAD_INPUT_LOGGER_H
#define KEYPAD_INPUT_LOGGER_H
#pragma once

namespace gh
{
	class KeyPadInputLogger
	{
	public:
		KeyPadInputLogger();

		void UpdateKeyStates();
		bool WasPreviouslyDown( const char& key ) const;
		bool WasJustPressed( const char& key ) const;
		bool IsKeyPressed( const char& key ) const;
		bool IsCharacterDown( const char& key ) const;
		bool CharacterWasJustPressed() const;
		bool CharacterIsDown() const;
		char GetLastPressedChar() const;
		char GetCharJustPressed() const;
		void onKeyDown( const char& key );
		void onKeyUp( const char& key );

	private:
		bool m_previousKeyStates[ 256 ];
		bool m_keyDown[ 256 ];
		int m_characterJustPressed;
		int m_lastCharacterDown;
	};
}

#endif