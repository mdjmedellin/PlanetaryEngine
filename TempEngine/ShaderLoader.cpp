#include <fstream>
#include <direct.h>
#include <string>
#include <Windows.h>
#include <cassert>

#include "ShaderLoader.h"
//#include "MemoryUtilities.h"
#include "FileManager.h"

namespace gh
{
	GLuint ShaderLoader::getShaderProgram( const char* vertexShaderFilePath, const char* fragmentShaderFilePath )
	{
		char* loadedFile = nullptr;
		GLint success = GL_FALSE;
		int length = 0;
		char* infoLog = NULL;
		char* buffer = NULL;

		GLuint shaderProgram = glCreateProgram();

		//check if we have created the vertex shader before
		auto iter = m_pathToShaders.find( vertexShaderFilePath );

		if( iter != m_pathToShaders.end() )
		{
			//we have previously created and compiled the shader with no problems
			//meaning that we can attach the shader without worrying
			glAttachShader( shaderProgram, iter->second );
		}
		else
		{
			//creating the vertex shader
			GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );

			//loading the file containing the vertex shader
			if( !FileManager::getSingleton().loadFile( vertexShaderFilePath, loadedFile, length ) )
			{
				return 0;
			}

			//compiling the vertex shader
			if( !compileShader( loadedFile, vertexShader, vertexShaderFilePath, shaderProgram, "GLSL vertex shader compile error!" ) )
			{
				delete[] loadedFile;
				loadedFile = nullptr;
				return 0;
			}
			else
			{
				//add the vertex shader to our list of loaded shaders
				m_pathToShaders[ vertexShaderFilePath ] = vertexShader;
			}

			delete[] loadedFile;
			loadedFile = nullptr;
		}

		//repeat the process for the fragment shader
		iter = m_pathToShaders.find( fragmentShaderFilePath );

		if( iter != m_pathToShaders.end() )
		{
			//we have previously created and compiles the shader with no problems
			//meaning that we can attach the shader without worrying
			glAttachShader( shaderProgram, iter->second );
		}
		else
		{
			//creating the fragment shader
			GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

			//loading the file containing the fragment shader
			if( !FileManager::getSingleton().loadFile( fragmentShaderFilePath, loadedFile, length ) )
			{
				return 0;
			}

			//compiling the fragment shader
			if( !compileShader( loadedFile, fragmentShader, fragmentShaderFilePath, shaderProgram, "GLSL fragment shader compile error!" ) )
			{
				delete[] loadedFile;
				loadedFile = nullptr;
				return 0;
			}
			else
			{
				//add the fragment shader to our list of loaded shaders
				m_pathToShaders[ fragmentShaderFilePath ] = fragmentShader;
			}

			delete[] loadedFile;
			loadedFile = nullptr;
		}

		//after attaching both shaders and compiling, link the program
		glLinkProgram( shaderProgram );
		glGetProgramiv( shaderProgram, GL_LINK_STATUS, &success );

		if( success != GL_TRUE )
		{
			//build error message
			glGetProgramiv( shaderProgram, GL_INFO_LOG_LENGTH, &length );
			infoLog = new char[ length + 1 ];
			glGetProgramInfoLog( shaderProgram, length, &length, infoLog );

			//DISPLAY LINKING ERROR
			// if failed to load file display error message
			std::string errorMessage = "Shader program linking error!\n";
			errorMessage = errorMessage + "\nFailure to link program making use of the following files: \n";
			errorMessage = errorMessage + "Vertex Shader File: " + vertexShaderFilePath + "\n";
			errorMessage = errorMessage + "Fragment Shader File: " + fragmentShaderFilePath + "\n";

			//raw error message
			errorMessage = errorMessage + "\nRAW ERROR MESSAGE: \n" + infoLog + "\n";
			errorMessage = errorMessage + "\nApplication Will Now Close\n";

			MessageBoxA( NULL, errorMessage.c_str(), "Linking Error", MB_ICONERROR );

			// Get the current working directory: 
			if( ( buffer = _getcwd( NULL, 0 ) ) == NULL )
				perror( "_getcwd error" );
			else
			{
				std::string vsConsoleMessage = buffer;
				vsConsoleMessage = vsConsoleMessage + "\\ShaderLoader.cpp\n";
				vsConsoleMessage = "> Shader Program Linking Error:\n> " + vsConsoleMessage;
				vsConsoleMessage = vsConsoleMessage + "> Failure to link vertex shader: \n> " + buffer + "\\" + vertexShaderFilePath + "\n";
				vsConsoleMessage = vsConsoleMessage + "> with fragment shader: \n> " + buffer + "\\" + fragmentShaderFilePath + "\n";
				vsConsoleMessage = vsConsoleMessage + "> " + infoLog + "\n";

				OutputDebugStringA( vsConsoleMessage.c_str() );
				free(buffer);
			}
			return 0;
		}

		//return the program
		return shaderProgram;
	}


	void ShaderLoader::createOutputError( char*& infoLog, std::string& filePath, char*& buffer )
	{
		//the fileName should be inserted before the first ( 
		size_t startIndex = 0;
		size_t charCount = 0;

		for( size_t index = 0; ; ++index )
		{
			if( *( infoLog + index ) == '(' && startIndex == 0 )
			{
				startIndex = index;
			}
			else if( *( infoLog + index ) == 0 )
			{
				charCount = index - startIndex;
				break;
			}
		}

		buffer = new char[ charCount + 1 ];
		memcpy( buffer, &infoLog[ startIndex ], charCount );
		buffer[ charCount ] = 0;

		filePath = filePath + buffer;
	}

	void ShaderLoader::extractLineNumber( char*& infoLog, char*& buffer )
	{
		//line number is contained inside the first pair of ()
		size_t startIndex = 0;
		size_t charsToCopy = 0;

		for( size_t i = 0; ; ++i )
		{
			if( *( infoLog + i ) == 0 )
			{
				break;
			}
			else
			{
				if( *( infoLog + i ) == '(' )
				{
					startIndex = i + 1;
				}
				else if( *( infoLog + i ) == ')' )
				{
					charsToCopy = i - startIndex;
				}
			}
		}

		buffer = new char[ charsToCopy + 1 ];

		memcpy( buffer, &infoLog[ startIndex ], charsToCopy );
		buffer[ charsToCopy ] = 0;
	}

	void ShaderLoader::retrieveError( char*& infoLog, char*& buffer )
	{
		//the actual text description of the error is found after the second colon
		// xxxxx:xxxxx:Textual description of error

		int colonCount = 2;

		size_t startIndex = 0;
		size_t charsToCopy = 0;

		for( size_t i = 0; ; ++i )
		{
			if( *( infoLog + i ) == 0 )
			{
				charsToCopy = i - startIndex;
				break;
			}
			else
			{
				if( *( infoLog + i ) == ':' )
				{
					--colonCount;

					if( colonCount == 0 )
					{
						startIndex = i+1;
					}
				}
			}
		}

		if( charsToCopy != 0 )
		{
			buffer = new char[ charsToCopy ];

			memcpy( buffer, &infoLog[ startIndex ], charsToCopy );
			buffer[ charsToCopy-1 ] = 0;
		}
	}

	int ShaderLoader::charStringToInteger( char*& digitString )
	{
		//does not handle negatives
		int number = 0;

		for( size_t i = 0; *( digitString + i ); ++i )
		{
			number *= 10;
			number += *( digitString + i ) - '0';
		}

		return number;
	}

	void ShaderLoader::getLine( char*& loadedFile, size_t lineNum, char*& buffer )
	{
		size_t startIndex = 0;
		size_t charsToCopy = 0;
		size_t currentLineNum = 1;


		for( size_t i = 0; ; ++i )
		{
			if( *( loadedFile + i ) == 0 )
			{
				if( currentLineNum == lineNum )
				{
					charsToCopy = i - startIndex;
				}
				break;
			}
			else
			{
				if( *( loadedFile + i ) == '\n' )
				{
					if( currentLineNum == lineNum )
					{
						charsToCopy = i - startIndex;
					}

					++currentLineNum;

					if( currentLineNum == lineNum )
					{
						startIndex = i+1;
					}
				}
			}
		}

		if( charsToCopy != 0 )
		{
			buffer = new char[ charsToCopy + 1 ];

			memcpy( buffer, &loadedFile[ startIndex ], charsToCopy );
			buffer[ charsToCopy ] = 0;
		}
	}

	bool ShaderLoader::loadAndCompileShader( char*& loadedFile, const GLuint& shader ) const
	{
		GLint success = GL_FALSE;

		//load the file into the shader and compile
		glShaderSource( shader, 1, ( const GLchar** ) &( loadedFile ), NULL );
		glCompileShader( shader );

		//check compilation status
		glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
		return success == GL_TRUE;
	}

	bool ShaderLoader::loadFile( const char* filePath, char*& contentHolder )
	{
		//opening a stream in order to extract the contents
		std::ifstream fileStream ( filePath, std::ifstream::binary );

		if ( fileStream.is_open() )
		{
			// get length of file:
			fileStream.seekg (0, fileStream.end);
			size_t length = ( size_t )fileStream.tellg();
			fileStream.seekg (0, fileStream.beg);

			//create a buffer big enough to hold the data
			if( contentHolder != NULL )
			{
				delete[] contentHolder;
			}
			contentHolder = new char [length + 1];

			// read data as a block:
			fileStream.read ( contentHolder, length );
			contentHolder[ length ] = 0;
			fileStream.close();

			return true;
		}
		else
		{
			// if failed to load file display error message
			std::string errorMessage;
			errorMessage = errorMessage + "Shader File Loading Error\n";
			errorMessage = errorMessage + "Failure To Load File: " + filePath + "\n\n";
			errorMessage = errorMessage + "Application Will Now Close\n";

			MessageBoxA( NULL, errorMessage.c_str(), "Failure To Load File", MB_ICONERROR );
			
			//display on visual studio output window
			char* buffer;
			// Get the current working directory: 
			if( ( buffer = _getcwd( NULL, 0 ) ) == NULL )
				perror( "_getcwd error" );
			else
			{
				std::string vsConsoleMessage = buffer;
				vsConsoleMessage = vsConsoleMessage + "\\ShaderLoader.cpp\n";
				vsConsoleMessage = "> File Read Error:\n> " + vsConsoleMessage;
				vsConsoleMessage = vsConsoleMessage + "> Failure to load file: " + filePath + "\n";
				OutputDebugStringA( vsConsoleMessage.c_str() );
				free(buffer);
			}

			return false;
		}
	}

	void ShaderLoader::create()
	{
		assert( !s_instance );
		s_instance.reset( new ShaderLoader );
	}

	void ShaderLoader::destroy()
	{
		assert( s_instance );

		for( auto iterator = s_instance->m_pathToShaders.begin(); iterator != s_instance->m_pathToShaders.end(); ++iterator)
		{
			glDeleteShader( iterator->second );
		}

		s_instance.reset();
	}

	ShaderLoader& ShaderLoader::instance()
	{
		assert( s_instance );
		return *s_instance;
	}

	bool ShaderLoader::compileShader( char* loadedFile, GLuint shader, const char* shaderFilePath, GLuint shaderProgram, const char* errorTittle )
	{
		int length = 0;
		char* buffer = nullptr;
		char* infoLog = nullptr;

		//checking if the loaded  shader failed to compile
		if( !loadAndCompileShader( loadedFile, shader ) )
		{
			//retrieving the error message
			glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &length );

			infoLog = new char[ length + 1 ];

			glGetShaderInfoLog(	shader, length, &length, infoLog);

			//error type
			std::string toOutput = errorTittle;
			toOutput = toOutput + "\n";

			//file failed to compile and line number
			toOutput = toOutput + "\n" + shaderFilePath + ", line ";

			extractLineNumber( infoLog, buffer );
			toOutput = toOutput + buffer + "\n";

			//extract line indicated by the error
			int lineNum = charStringToInteger( buffer );
			delete[] buffer;
			buffer = NULL;

			getLine( loadedFile, lineNum, buffer );
			
			//remove white spaces from the beginning of the line
			removeWSFront( buffer );
			toOutput = toOutput + "> " + buffer + "\n";

			//error
			delete[] buffer;
			buffer = NULL;

			retrieveError( infoLog, buffer );
			toOutput = toOutput + "\nERROR: " + buffer + "\n";

			//current OpenGL version
			toOutput = toOutput + "\nCurrent OpenGL version: " + ( char* )glGetString( GL_VERSION ) + "\n";

			//raw error message
			toOutput = toOutput + "RAW ERROR MESSAGE:\n";
			toOutput = toOutput + infoLog + "\n";

			//application will now close
			toOutput = toOutput + "\nThe application will now close.\n";

			//visual studio output
			// Get the current working directory:
			delete[] buffer;
			buffer = NULL;
			if( ( buffer = _getcwd( NULL, 0 ) ) == NULL )
				perror( "_getcwd error" );
			else
			{
				std::string vsOutput = errorTittle;
				vsOutput = "> " + vsOutput + "\n> " + buffer + "\\" + shaderFilePath;
				
				delete[] buffer;
				buffer = NULL;
				createOutputError( infoLog, vsOutput, buffer );

				OutputDebugStringA( vsOutput.c_str() );
				delete[] buffer;
				buffer = NULL;
			}

			//pop up window
			MessageBoxA( NULL, toOutput.c_str(), errorTittle, MB_ICONERROR );
			
			//clean up
			if( infoLog )
			{
				delete[] infoLog;
				infoLog = NULL;
			}

			return false;
		}
		else
		{
			glAttachShader( shaderProgram, shader );
			return true;
		}
	}

	void ShaderLoader::removeWSFront( char*& line )
	{
		size_t startIndex = 0;
		size_t charsToCopy = 0;
		char* substring = nullptr;

		//check if there is any whitespace at the beginning
		if( *( line + startIndex ) != ' ' )
		{
			return;
		}
		else
		{
			for( size_t index = 1; ; ++index )
			{
				if( *( line + index ) == 0 )
				{
					charsToCopy = index - startIndex;
					break;
				}
				else if( startIndex == 0 && *( line + index ) != ' ' )
				{
					startIndex = index;
				}
			}

			if( charsToCopy != 0 )
			{
				substring = new char[ charsToCopy + 1 ];
				strcpy ( substring, &line[ startIndex ] );

				delete[] line;
				line = substring;
			}
		}
	}

	std::unique_ptr< ShaderLoader > ShaderLoader::s_instance;
}