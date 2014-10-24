#include "ZipFileLoader.h"
#include "ZipFileManager.h"
#include <algorithm>

namespace gh
{
	bool ZipFileLoader::loadFile( const char* filePath, char*& out_fileBuffer, int& out_bufferSize )
	{
		std::string fullFilePath = filePath;
		std::replace( fullFilePath.begin(), fullFilePath.end(), '\\', '/');

		if( m_zipFolder )
		{
			if( m_zipFolder->openFile( fullFilePath.c_str(), m_password ) )
			{
				size_t fileSize = m_zipFolder->getFileSizeOfOpenFile();

				if( out_fileBuffer != nullptr )
				{
					delete[] out_fileBuffer;
				}

				out_bufferSize = fileSize + 1;
				out_fileBuffer = new char[ out_bufferSize ];				//note, this is the array of characters that is passed back to the caller

				if( out_fileBuffer )
				{
					m_zipFolder->getFileContents( out_fileBuffer, fileSize + 1 );
				}
				else
				{
					return false;
				}

				m_zipFolder->closeCurrentFile();

				return true;
			}
		}
		else
		{
			return false;
		}
	}

	ZipFileLoader::ZipFileLoader( const char* filePath,  const char* password )
		:	m_password( password )
	{
		m_zipFolder = theZipFileManager.getZipFile( filePath );
	}

}