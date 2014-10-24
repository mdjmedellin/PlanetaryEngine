#pragma once
#ifndef SHADERLOADER_H
#define SHADERLOADER_H

//includes
#include <map>
#include <glew.h>
//

namespace gh
{
	class ShaderLoader
	{
	public:
		GLuint getShaderProgram( const char* vertexShaderFilePath, const char* fragmentShaderFilePath );
		static void create();
		static void destroy();
		static ShaderLoader& instance();

	protected:
		std::map< std::string, GLuint > m_pathToShaders;
		static std::unique_ptr< ShaderLoader > s_instance;

	private:
		bool loadFile( const char* filePath, char*& contentHolder );
		bool loadAndCompileShader( char*& loadedFile, const GLuint& shader ) const;
		int charStringToInteger( char*& digitString );
		void extractLineNumber( char*& infoLog, char*& buffer );
		void getLine( char*& loadedFile, size_t lineNum, char*& buffer );
		void retrieveError( char*& infoLog, char*& buffer );
		void createOutputError( char*& infoLog, std::string& filePath, char*& buffer );
		void removeWSFront( char*& line );
		bool compileShader( char* loadedFile, GLuint shader, const char* shaderFilePath, GLuint shaderProgram, const char* errorTittle );
	};
}

#endif