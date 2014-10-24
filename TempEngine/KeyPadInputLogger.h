#ifndef KEYPAD_INPUT_LOGGER_H
#define KEYPAD_INPUT_LOGGER_H
#pragma once

namespace gh
{
	class KeyPadInputLogger
	{
	public:
		KeyPadInputLogger();

		bool isKeyPressed( const char& key ) const;
		void onKeyDown( const char& key );
		void onKeyUp( const char& key );

	private:
		bool m_keyDown[ 256 ];
	};
}

#endif