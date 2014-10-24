#pragma once
#ifndef COMMANDCONSOLESYSTEM_H
#define COMMANDCONSOLESYSTEM_H

//includes
#include <map>
#include <vector>
#include "PredictionNode.h"
#include "Renderer.h"
//

//forward declarations
namespace gh
{
	class NamedProperties;
}

class AABB2;

struct ConsoleCommandArgs
{
	std::vector< std::string > m_argumentsVector;
	std::string m_argumentsString;

	ConsoleCommandArgs( const char* argumentString )
	{
		if( argumentString )
		{
			int startIndex = -1;
			int endIndex = -1;
			for( int i = 0; ; ++i )
			{
				//if the character is the null terminating character
				if( *( argumentString + i ) == '\0' )
				{
					if( endIndex != startIndex )
					{
						endIndex = i;
						m_argumentsVector.push_back( std::string( ( argumentString + startIndex ), ( argumentString + endIndex ) ) );
						startIndex = endIndex;
					}

					break;
				}

				//if the character is a space
				if( isspace( *( argumentString + i ) ) )
				{
					//if the character found is a space, and start and end are not pointing to the same location
					//then
					//	we know that startIndex has moved, which only happens when the a nonspace character is found and both pointers are in the same location
					//	and that we have found the end of the current argument
					if( endIndex != startIndex )
					{
						//if end index and start index are not the same, then we know we moved the start index to an actual character
						endIndex = i;
						m_argumentsVector.push_back( std::string( ( argumentString + startIndex), ( argumentString + endIndex ) ) );
						startIndex = endIndex;
					}
				}
				else
				{
					if( endIndex == startIndex )
					{
						startIndex = i;
					}
				}
			}
		}

		m_argumentsString = argumentString;
	}
};

typedef void ( *ConsoleCommandFunctionPtr )( const ConsoleCommandArgs& args );

class CommandConsoleSystem
{
	typedef std::map< std::string, ConsoleCommandFunctionPtr >::iterator mapTypeIterator;

public:
	void init( const std::string& commandPrompt, const std::string& fontName, float fontHeight );
	void render( const AABB2& renderWindow );
	void registerConsoleCommand( const std::string& cmdName, ConsoleCommandFunctionPtr CommandFunction );
	void printToConsole( const std::string& stringToPrint, unsigned int tint );
	void printNewlineToConsole( const std::string& stringToPrint, unsigned int tint );
	void useFont( const std::string& renderFont, float fontHeight = 12  );
	void clearScreen();
	void setCommandPrompt( const std::string& commandPrompt );
	void keyPressed( size_t key );
	bool aSyncKeyPressed();
	void recalculateText();
	bool executeConsoleCommandString( const std::string& commandString );

private:
	bool executeConsoleCommand( const std::string& cmdName, const ConsoleCommandArgs& args );
	void extractCommandAndArgument( const std::string& commandString, std::string& commandName, std::string& argument );
	void inputLineBackSpace();
	void clearCurrentInput();
	void processInputLine();
	void moveDownHistoryLines();
	void moveUpHistoryLines();
	void moveCursorRight();
	void moveCursorLeft();
	void printNewlineToConsoleEventFunction( const gh::NamedProperties& arguments );

	std::map< std::string, ConsoleCommandFunctionPtr > m_registeredCommands;
	bool m_hasBeenModified;
	std::vector< CommandConsoleText > m_textLines, m_inputLines;
	std::vector< Vertex2PTC >m_textDataArray, m_inputTextDataArray;
	std::string m_fontName;
	float m_fontHeight;
	std::string m_commandPrompt, m_currentInput;
	size_t m_currentHistoryLine;
	size_t m_currentInputLocation;
	std::shared_ptr< gh::PredictionNode >m_predictionRoot;
	std::vector<std::string> m_predictions;
};

extern CommandConsoleSystem theConsole;

#endif