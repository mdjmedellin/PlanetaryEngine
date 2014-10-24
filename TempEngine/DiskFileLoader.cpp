#include "DiskFileLoader.h"
#include <fstream>
#include <direct.h>

namespace gh
{
	bool DiskFileLoader::loadFile( const char* filePath, char*& out_fileBuffer, int& bufferSize )
	{
		std::string fullFilePath = m_diskToDataFolder + "\\";
		fullFilePath.append( filePath );
		std::ifstream fileStream ( fullFilePath, std::ifstream::binary );

		if ( fileStream.is_open() )
		{
			// get length of file:
			fileStream.seekg (0, fileStream.end);
			size_t length = ( size_t )fileStream.tellg();
			fileStream.seekg (0, fileStream.beg);

			//create a buffer big enough to hold the data
			if( out_fileBuffer != nullptr )
			{
				delete[] out_fileBuffer;
			}
			bufferSize = length + 1;
			out_fileBuffer = new char[ length + 1 ];				//note, this is the array of characters that is passed back to the caller

			// read data as a block:
			fileStream.read ( out_fileBuffer, length );
			out_fileBuffer[ length ] = 0;
			fileStream.close();

			return true;
		}
		else
		{
			return false;
			/*// if failed to load file display error message
			std::string errorMessage;
			errorMessage = errorMessage + "Shader File Loading Error\n";
			errorMessage = errorMessage + "Failure To Load File: " + filePath + "\n\n";
			errorMessage = errorMessage + "Application Will Now Close\n";

			MessageBoxA( NULL, errorMessage.c_str(), "Failure To Load File", MB_ICONERROR );

			//display on visual studio output windos
			std::string vsConsoleMessage( filePath );
			vsConsoleMessage = vsConsoleMessage + "\n";
			vsConsoleMessage = "> File Read Error:\n> " + vsConsoleMessage;
			vsConsoleMessage = vsConsoleMessage + "> Failure to load file: " + filePath + "\n";
			OutputDebugStringA( vsConsoleMessage.c_str() );*/
		}
	}

	DiskFileLoader::DiskFileLoader( const std::string& diskDataFolderPath )
		:	m_diskToDataFolder( diskDataFolderPath )
	{}

}