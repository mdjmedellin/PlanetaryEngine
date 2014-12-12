#include <Windows.h>
#include <algorithm>
#include <direct.h>

#include "CommandConsoleSystem.h"
#include "AABB2.hpp"
#include "MathUtilities.hpp"
#include "TimeUtility.h"
#include "Matrix4X4.h"
#include "Texture.h"
#include "Renderer.h"
#include "Vector4.h"
#include "NamedProperties.h"
#include "EventSystem.h"
#include "Rgba.h"
#include "MutexLock.h"
#include "Mutex.h"

//#include "MemoryUtilities.h"

CommandConsoleSystem theConsole;

void CommandConsoleSystem::init( const std::string& commandPrompt, const std::string& fontName, float fontHeight )
{
	m_hasBeenModified = false;
	m_fontName = fontName;
	m_fontHeight = fontHeight;
	m_commandPrompt = commandPrompt;
	m_currentInput = "";
	m_currentHistoryLine = 0;
	m_currentInputLocation = 0;
	m_predictionRoot.reset( new gh::PredictionNode );

	theEventSystem.Register( this, &CommandConsoleSystem::printNewlineToConsoleEventFunction, "printNewlineToConsole" );
}

void CommandConsoleSystem::registerConsoleCommand( const std::string& cmdName, ConsoleCommandFunctionPtr CommandFunction )
{
	mapTypeIterator iter = m_registeredCommands.find( cmdName );

	if( iter == m_registeredCommands.end() )
	{
		m_registeredCommands.insert( std::pair< std::string, ConsoleCommandFunctionPtr >( cmdName, CommandFunction ) );
		m_predictionRoot->addChild( cmdName );
	}
	else
	{
		std::string errorMessage = "Function " + cmdName + " has already been registered on the console\n\n"
									+ "The function will not be overridden\n\n";
		MessageBoxA( NULL, errorMessage.c_str(), "Function Redefinition", MB_ICONERROR );
	}
}

bool CommandConsoleSystem::executeConsoleCommand( const std::string& cmdName, const ConsoleCommandArgs& args )
{
	std::string cmdNameLower = cmdName;
	std::transform( cmdName.begin(), cmdName.end(), cmdNameLower.begin(), ::tolower );

	mapTypeIterator iter = m_registeredCommands.find( cmdNameLower );

	if( iter != m_registeredCommands.end() )
	{
		iter->second( args );
		return true;
	}
	else
	{
		printNewlineToConsole( "Command " + cmdName + " is not registered on the console", toRGBA( 1.f, 0.f, 0.f ) );
		return false;
	}
}

void CommandConsoleSystem::printToConsole( const std::string& textLine, unsigned int tint )
{
	//when printed to console, the input string is attached to the end of the last string
	if( m_textLines.empty() )
	{
		m_textLines.push_back( CommandConsoleText( textLine, tint ) );
	}
	else
	{
		m_textLines.back().m_textLine.append( textLine );
	}

	m_hasBeenModified = true;
}

void CommandConsoleSystem::clearScreen()
{
	m_hasBeenModified = false;
	m_textLines = std::vector< CommandConsoleText >();
	m_textDataArray = std::vector< Vertex2PTC >();
}

void CommandConsoleSystem::printNewlineToConsole( const std::string& textLine, unsigned int tint )
{
	static gh::Mutex myMutex;
	gh::MutexLock functionMutexLock(myMutex);
	//when printedNewlineToConsole
	//the text to be printed is done with a newline preceding it
	m_textLines.push_back( CommandConsoleText( textLine, tint ) );
	m_hasBeenModified = true;
}

void CommandConsoleSystem::render( const AABB2& renderWindow )
{
	AABB2 renderWindowMinusInput = renderWindow;
	AABB2 renderWindowInputLine = renderWindow;
	
	//we leave the bottom portion of the command console for the user's input
	//Check if we have enough space for past input lines
	if( renderWindow.getMaxs().y - renderWindow.getMins().y > ( m_fontHeight * 2.0f ) )
	{
		renderWindowMinusInput.setMins( renderWindow.getMins().x, renderWindow.getMins().y + ( m_fontHeight * 2.0f ) );
		renderWindowInputLine.setMaxs( renderWindow.getMaxs().x, renderWindowMinusInput.getMins().y );
		//render the black box that is behind the text
		theRenderer.renderQuad( renderWindowMinusInput, Vector4( 0.f, 0.f, 0.f, 0.8f ), nullptr );
	}

	//render the command input section a grayish color
	theRenderer.renderQuad( renderWindowInputLine, Vector4( 0.2f, 0.2f, 0.2f, 0.9f ), nullptr );

	//check if the data has been modified
	if( m_hasBeenModified )
	{
		m_textDataArray = theRenderer.generateTextDataArray( m_textLines, m_fontName, m_fontHeight, renderWindowMinusInput, TEXT_ALIGN_BOTTOM, 1 );
		m_hasBeenModified = false;
	}

	//draw the input line
	theRenderer.drawText( m_currentInput.c_str(), m_fontName, m_fontHeight, renderWindowInputLine, BOX_ALIGN, toRGBA( 1.f, 1.f, 1.f ) );

	//create box in which we are to draw the blinking cursor
	static bool drawCursor = false;
	float blinkingCursorOffsetX = theRenderer.getFont( m_fontName )->calcTextPixelWidth( m_currentInput.substr( 0, m_currentInputLocation ).c_str(), m_fontHeight );
	AABB2 blinkingCursorBox = renderWindowInputLine;
	blinkingCursorBox.setMins( blinkingCursorBox.getMins().x + blinkingCursorOffsetX, blinkingCursorBox.getMins().y );

	//determining whether the cursor should be drawn
	static double nextTime = gh::GetAbsoluteTimeSeconds() + 1.0;
	if( gh::GetAbsoluteTimeSeconds() > nextTime )
	{
		drawCursor = !drawCursor;
		nextTime = gh::GetAbsoluteTimeSeconds() + 1.0f;
	}

	if( drawCursor )
	{
		theRenderer.drawText( "_", m_fontName, m_fontHeight, blinkingCursorBox, BOX_ALIGN, toRGBA( 1.f, 1.f, 1.f ) );
	}

	//draw the prediction on top of the cursor
	if( !m_predictions.empty() )
	{
		blinkingCursorBox.setMins( blinkingCursorBox.getMins().x, blinkingCursorBox.getMins().y+m_fontHeight );
		blinkingCursorBox.setMaxs( blinkingCursorBox.getMaxs().x, blinkingCursorBox.getMaxs().y+m_fontHeight );
		theRenderer.drawText( m_predictions.back().c_str(), m_fontName, m_fontHeight, blinkingCursorBox, BOX_ALIGN, toRGBA( 1.f, .7f, .27f ) );
	}

	//render the textDataArray
	if( !m_textDataArray.empty() )
	{
		theRenderer.renderDataArrayV2PTC( m_textDataArray, m_fontName );
	}
}

void CommandConsoleSystem::useFont( const std::string& fontName, float fontHeight /* = 12 */ )
{
	m_fontName = fontName;
	m_fontHeight = fontHeight;
}

void CommandConsoleSystem::keyPressed( size_t key )
{
	if( isprint( key ) )
	{
		//checking where we are going to add the new character
		if( !m_currentInput.empty() || m_currentInputLocation != m_currentInput.size() )
		{
			m_currentInput.insert( m_currentInput.begin() + m_currentInputLocation, char( key ) );
		}
		else
		{
			m_currentInput += char( key );
		}

		//this is very rough, ALPHA
		m_predictions.clear();
		m_predictionRoot->getTopXList( m_predictions, 1, m_currentInput );

		++m_currentInputLocation;
	}
}

void CommandConsoleSystem::recalculateText()
{
	m_hasBeenModified = true;
}

void CommandConsoleSystem::setCommandPrompt( const std::string& commandPrompt )
{
	m_commandPrompt = commandPrompt;
}

bool CommandConsoleSystem::executeConsoleCommandString( const std::string& commandString )
{
	//don't do anything if the line is pure whitespace
	if( commandString.find_first_not_of(" \n\r\t") == std::string::npos )
	{
		m_textLines.push_back( CommandConsoleText( "", toRGBA( 1.f, 1.f, 1.f ) ) );
		m_hasBeenModified = true;
		m_currentInputLocation = 0;
		return false;
	}

	std::string commandName;
	std::string argumentsString;
	extractCommandAndArgument( commandString, commandName, argumentsString );

	return executeConsoleCommand( commandName, ConsoleCommandArgs( argumentsString.c_str() ) );
}

void CommandConsoleSystem::extractCommandAndArgument( const std::string& commandString, std::string& commandName, std::string& argument )
{
	//finding the beginning of the command name, no whitespace
	size_t cmdNameStart = commandString.find_first_not_of( " \n\r\t" );

	if( cmdNameStart == std::string::npos )
	{
		//all whitespace
		return;
	}
	else
	{
		commandName = commandString.substr( cmdNameStart );
	}

	//finding the end of the command name
	size_t cmdNameEnd = commandName.find_first_of( " \n\r\t" );
	
	if( cmdNameEnd == std::string::npos )
	{
		//empty argumentsString
		argument = '\0';
		return;
	}
	else
	{
		commandName = commandName.substr( 0, cmdNameEnd );
	}

	//extracting the argument part of the string
	argument = commandString.substr( cmdNameStart + cmdNameEnd );
}

void CommandConsoleSystem::moveCursorLeft()
{
	if( m_currentInputLocation != 0 )
	{
		--m_currentInputLocation;
	}
}

void CommandConsoleSystem::moveCursorRight()
{
	if( m_currentInputLocation != m_currentInput.size() )
	{
		++m_currentInputLocation;
	}
}

void CommandConsoleSystem::moveUpHistoryLines()
{
	//code to move through previous input
	if( m_currentHistoryLine != 0 )
	{
		--m_currentHistoryLine;
	}
	if( !m_inputLines.empty() )
	{
		m_currentInput = m_inputLines[ m_currentHistoryLine ].m_textLine;
		m_currentInputLocation = m_currentInput.size();
	}

	m_predictions.clear();
}

void CommandConsoleSystem::moveDownHistoryLines()
{
	//code to move through history lines
	if( m_currentHistoryLine < m_inputLines.size() )
	{
		++m_currentHistoryLine;

		if( m_currentHistoryLine == m_inputLines.size() )
		{
			m_currentInput.clear();
		}
		else
		{
			m_currentInput = m_inputLines[ m_currentHistoryLine ].m_textLine;
		}
		
		m_currentInputLocation = m_currentInput.size();
	}

	m_predictions.clear();
}

void CommandConsoleSystem::processInputLine()
{
	//check if the line is valid for processing
	if( executeConsoleCommandString( m_currentInput ) )
	{
		m_textLines.push_back( CommandConsoleText( m_commandPrompt + m_currentInput, toRGBA( 1.f, 1.f, 0 ) ) );
		m_inputLines.push_back( CommandConsoleText( m_currentInput, toRGBA(1.f, 1.f, 1.f ) ) );

		m_hasBeenModified = true;

		m_currentHistoryLine = m_inputLines.size();
	}

	clearCurrentInput();
}

void CommandConsoleSystem::clearCurrentInput()
{
	m_currentInput.clear();
	m_currentInputLocation = 0;
	m_predictions.clear();
}

void CommandConsoleSystem::inputLineBackSpace()
{
	if( m_currentInput.size() > 0 && m_currentInputLocation != 0 )
	{
		m_currentInput.erase( m_currentInput.begin() + m_currentInputLocation - 1 );

		--m_currentInputLocation;
	}

	m_predictions.clear();
	m_predictionRoot->getTopXList( m_predictions, 1, m_currentInput );
}

bool CommandConsoleSystem::aSyncKeyPressed()
{
	bool usingKey = true;

	if( GetAsyncKeyState( VK_LEFT ) )
	{
		moveCursorLeft();
	}
	else if( GetAsyncKeyState( VK_RIGHT ) )
	{
		moveCursorRight();
	}
	else if( GetAsyncKeyState( VK_UP ) )
	{
		moveUpHistoryLines();
	}
	else if( GetAsyncKeyState( VK_DOWN ) )
	{
		moveDownHistoryLines();
	}
	else if( GetAsyncKeyState( VK_RETURN ) )
	{
		processInputLine();
	}
	else if( GetAsyncKeyState( VK_BACK ) )
	{
		inputLineBackSpace();
	}
	else if( GetAsyncKeyState( VK_ESCAPE ) )
	{
		clearCurrentInput();
	}
	else if( GetAsyncKeyState( VK_TAB ) )
	{
		if( !m_predictions.empty() )
		{
			m_currentInput = m_predictions.back();
			m_currentInputLocation = m_currentInput.length();
		}
	}
	else
	{
		//this is the default stage, meaning that the console is not using the key
		usingKey = false;
	}

	return usingKey;
}

void CommandConsoleSystem::printNewlineToConsoleEventFunction( const gh::NamedProperties& arguments )
{
	//extracting the arguments
	std::string outputText;
	unsigned int outputColor;

	if ( arguments.getProperty( "outputText", outputText ) == gh::PROPERTY_GET_SUCCESS &&
		 arguments.getProperty( "outputColor", outputColor ) == gh::PROPERTY_GET_SUCCESS )
	{
		m_textLines.push_back( CommandConsoleText( outputText, outputColor ) );
		m_hasBeenModified = true;
	}
}
