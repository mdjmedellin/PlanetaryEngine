#pragma once
#ifndef ZIP_FILE_H
#define ZIP_FILE_H

//includes
#ifdef unix
# include <unistd.h>
# include <utime.h>
#else
# include <direct.h>
# include <io.h>
#endif

#include "unzip.h"

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

#include <vector>
//

namespace gh
{
	class ZipFile
	{
	private:
		struct FileHierarchy
		{
		private:
			std::pair< size_t, const char* >m_fileName;
			std::vector< std::pair< size_t, const char* > >m_fileHierarchy;
			size_t m_currentFilePathSize;

		public:
			FileHierarchy()
				:	m_currentFilePathSize( 0 )
				,	m_fileHierarchy( std::vector< std::pair< size_t, const char* > >() )
				,	m_fileName( std::pair< size_t, const char* >( 0, "" ) )
			{}

			size_t getHierarchyPathSize()
			{
				return m_currentFilePathSize;
			}

			void goDownTo( const char* subfolderName )
			{
				size_t charCount = 0;

				for( ; subfolderName[charCount]; charCount++ )
				{}

				if( charCount != 0 )
				{
					m_fileHierarchy.push_back( std::pair< size_t, const char* >( charCount, subfolderName ) );
					m_currentFilePathSize += charCount + 1;
				}
			}

			size_t getFilePathLength( const char* fileName )
			{
				size_t charCount = 0;

				for( ; fileName[charCount]; charCount++ )
				{}

				if( charCount != 0 )
				{
					return m_currentFilePathSize + charCount;
				}
			}

			void getFilePath( const char* fileName, char* out_FilePath, size_t charactersInFilePath )
			{
				if( charactersInFilePath == 0 )
				{
					return;
				}

				//build the hierarchy path
				size_t charactersCopied = 0;

				for( std::vector< std::pair< size_t, const char* > >::iterator currentSubfolder = m_fileHierarchy.begin();
					currentSubfolder != m_fileHierarchy.end(); ++currentSubfolder )
				{
					memcpy( &out_FilePath[charactersCopied], &(currentSubfolder->second[0]), currentSubfolder->first );
					out_FilePath[ currentSubfolder->first ] = '/';
					charactersCopied += currentSubfolder->first + 1;
				}

				//add the fileName at the end
				memcpy( &out_FilePath[ charactersCopied ], fileName, charactersInFilePath - charactersCopied );

				//add the null terminating character
				out_FilePath[ charactersInFilePath ] = '\0';
			}

			void goUp()
			{
				if( !m_fileHierarchy.empty() )
				{
					m_currentFilePathSize -= ( m_fileHierarchy.back().first + 1 );
					m_fileHierarchy.pop_back();
				}
			}

			void getHierarchyPath( char* out_HierarchyPath )
			{
				if( !m_fileHierarchy.empty() )
				{
					//build the hierarchy path
					size_t charactersCopied = 0;

					for( std::vector< std::pair< size_t, const char* > >::iterator currentSubfolder = m_fileHierarchy.begin();
						currentSubfolder != m_fileHierarchy.end(); ++currentSubfolder )
					{
						memcpy( &out_HierarchyPath[charactersCopied], &(currentSubfolder->second[0]), currentSubfolder->first );
						out_HierarchyPath[ currentSubfolder->first ] = '/';
						charactersCopied += currentSubfolder->first + 1;
					}

					//add the null terminating character
					out_HierarchyPath[ m_currentFilePathSize ] = '\0';
				}
			}
		};


	public:
		ZipFile( const char* zipFileName );
		bool isGood();
		bool goToFolder( const char* folderName );
		bool openFile( const char* fileName, const char* paswword = NULL );
		void closeCurrentFile();
		size_t getFileSizeOfOpenFile();
		bool getFileContents( char*& out_fileContentsBuffer, size_t bufferSize );

	private:
		void openZipFile();

		const char* m_zipFileName;
		const char* m_password;
		unzFile m_uFile;
		bool m_goodStatus;
		bool m_fileIsOpen;
		size_t m_fileSizeOfOpenFile;
		FileHierarchy m_folderStructure;
	};
}

#endif