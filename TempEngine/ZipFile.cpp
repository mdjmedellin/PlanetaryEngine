#include "ZipFile.h"

#include "MemoryUtilities.h"

namespace gh
{
	ZipFile::ZipFile( const char* zipFileName )
		:	m_zipFileName( zipFileName )
		,	m_password( NULL )
		,	m_uFile( NULL )
		,	m_goodStatus( false )
		,	m_fileIsOpen( false )
		,	m_fileSizeOfOpenFile( 0 )
	{
		if( m_zipFileName )
		{
			openZipFile();
		}
	}

	void ZipFile::openZipFile()
	{
		//checking to see if we can open the zip file to access its contents
#ifdef USEWIN32IOAPI
		zlib_filefunc64_def ffunc;
#endif
		char filename_try[MAXFILENAME+16] = "";
		strncpy( filename_try, m_zipFileName, MAXFILENAME-1 );
		filename_try[ MAXFILENAME ] = '\0';

#ifdef USEWIN32IOAPI
		fill_win32_filefunc64A( &ffunc );
		m_uFile = unzOpen2_64( m_zipFileName, &ffunc );
#else
		m_uFile = unzOpen64( m_zipFileName );
#endif
		if ( m_uFile == NULL )
		{
			strcat( filename_try, ".zip" );
#ifdef USEWIN32IOAPI
		m_uFile = unzOpen2_64( filename_try, &ffunc );
#else
		m_uFile = unzOpen64( filename_try );
#endif
		}

		m_goodStatus = ( m_uFile != NULL );
	}

	bool ZipFile::isGood()
	{
		return m_goodStatus;
	}

	bool ZipFile::goToFolder( const char* folderName )
	{
		closeCurrentFile();

		m_folderStructure.goDownTo( folderName );
		size_t currentHierarchyPathSize = m_folderStructure.getHierarchyPathSize();
		if( currentHierarchyPathSize == 0 )
		{
			return true;
		}
		else
		{
			if( unzGoToFirstFile( m_uFile ) == UNZ_OK )
			{
				//need to delete
				char* filePath_try = new char[ currentHierarchyPathSize + 1 ];
				m_folderStructure.getHierarchyPath( filePath_try );

				//need to delete
				char* buff = new char[ currentHierarchyPathSize + 1 ];

				bool keepLooking = true;
				bool foundMatch = false;

				do 
				{
					//create a unz_file_info instance and set value to zero
					unz_file_info fileInfo;
					memset( &fileInfo, 0, sizeof( unz_file_info ) );

					if ( unzGetCurrentFileInfo( m_uFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) 
					{
						//get the currentFileName
						char *filename = (char *)malloc( fileInfo.size_filename + 1 );
						unzGetCurrentFileInfo( m_uFile, &fileInfo, filename, fileInfo.size_filename + 1, NULL, 0, NULL, 0);
						filename[ fileInfo.size_filename ] = '\0';

						//check if the initial path of the filename matches
						if( fileInfo.size_filename >= currentHierarchyPathSize )
						{
							memcpy( buff, filename, currentHierarchyPathSize );
							buff[ currentHierarchyPathSize ] = '\0';

							if( unzStringFileNameCompare( buff, filePath_try, 0 ) == 0 )
							{
								//the paths are the same and we are on the first file of this kind
								keepLooking = false;
								foundMatch = true;
							}
						}

						free(filename);
					}

					unzCloseCurrentFile( m_uFile );

				} while ( keepLooking && unzGoToNextFile( m_uFile ) == UNZ_OK );

				//deleting buff
				delete[] buff;

				//deleting filePath_try
				delete[] filePath_try;

				if( !foundMatch )
				{
					m_folderStructure.goUp();
				}

				return foundMatch;
			}
			else
			{
				return false;
			}
		}

	}

	bool ZipFile::openFile( const char* fileName, const char* password )
	{
		closeCurrentFile();
		size_t fileSize = 0;
		
		size_t currentHierarchyPathSize = m_folderStructure.getHierarchyPathSize();
		if( currentHierarchyPathSize == 0 )
		{
			//search through all the files in the folder
			if( unzGoToFirstFile( m_uFile ) == UNZ_OK )
			{
				bool keepLooking = true;
				bool foundMatch = false;

				do 
				{
					//create a unz_file_info instance and set value to zero
					unz_file_info fileInfo;
					memset( &fileInfo, 0, sizeof( unz_file_info ) );

					if ( unzGetCurrentFileInfo( m_uFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) 
					{
						//get the currentFileName
						char *filename = (char *)malloc( fileInfo.size_filename + 1 );
						unzGetCurrentFileInfo( m_uFile, &fileInfo, filename, fileInfo.size_filename + 1, NULL, 0, NULL, 0);
						filename[ fileInfo.size_filename ] = '\0';

						if( unzStringFileNameCompare( fileName, filename, 0 ) == 0 )
						{
							m_fileSizeOfOpenFile = fileInfo.uncompressed_size;
							//the paths are the same and we are on the first file of this kind
							keepLooking = false;
							foundMatch = true;
						}

						free(filename);
					}
					unzCloseCurrentFile( m_uFile );
				} while ( keepLooking && unzGoToNextFile( m_uFile ) == UNZ_OK );

				//open the file
				if( foundMatch && unzOpenCurrentFilePassword( m_uFile, password ) == UNZ_OK )
				{
					m_fileIsOpen = true;
				}
				else
				{
					m_fileIsOpen = false;
				}
			}
		}
		else
		{
			//search for the file in the specified folder
			//need to delete
			size_t currentFilePathSize = m_folderStructure.getFilePathLength( fileName );
			char* filePath_try = new char[ currentFilePathSize + 1 ];
			m_folderStructure.getFilePath( fileName, filePath_try, currentFilePathSize );


			//need to delete
			char* buff = new char[ currentFilePathSize + 1 ];

			bool keepLooking = true;
			bool foundMatch = false;

			do 
			{
				//create a unz_file_info instance and set value to zero
				unz_file_info fileInfo;
				memset( &fileInfo, 0, sizeof( unz_file_info ) );

				if ( unzGetCurrentFileInfo( m_uFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) 
				{
					//get the currentFileName
					char *filename = (char *)malloc( fileInfo.size_filename + 1 );
					unzGetCurrentFileInfo( m_uFile, &fileInfo, filename, fileInfo.size_filename + 1, NULL, 0, NULL, 0);
					filename[ fileInfo.size_filename ] = '\0';

					//check if the initial path of the filename matches
					if( fileInfo.size_filename == currentFilePathSize )
					{
						memcpy( buff, filename, currentFilePathSize );
						buff[ currentFilePathSize ] = '\0';

						if( unzStringFileNameCompare( buff, filePath_try, 0 ) == 0 )
						{
							m_fileSizeOfOpenFile = fileInfo.uncompressed_size;
							//the paths are the same and we are on the first file of this kind
							keepLooking = false;
							foundMatch = true;
						}
					}

					free(filename);
				}

				unzCloseCurrentFile( m_uFile );

			} while ( keepLooking && unzGoToNextFile( m_uFile ) == UNZ_OK );

			//deleting buff
			delete[] buff;

			//deleting filePath_try
			delete[] filePath_try;

			//open the file
			if( foundMatch && unzOpenCurrentFilePassword( m_uFile, password ) == UNZ_OK )
			{
				m_fileIsOpen = true;
			}
			else
			{
				m_fileIsOpen = false;
			}
		}

		return m_fileIsOpen;
	}

	void ZipFile::closeCurrentFile()
	{
		if( m_fileIsOpen )
		{
			unzCloseCurrentFile( m_uFile );
			m_fileSizeOfOpenFile = 0;
			m_fileIsOpen = false;
		}
	}

	size_t ZipFile::getFileSizeOfOpenFile()
	{
		return m_fileSizeOfOpenFile;
	}

	bool ZipFile::getFileContents( char*& out_fileContentsBuffer, size_t bufferSize )
	{
		if( m_fileIsOpen )
		{
			void* fileContents = (void*)malloc( bufferSize );
			int status = unzReadCurrentFile( m_uFile, fileContents, bufferSize );
			char* test = (char*)( fileContents );
			memcpy( out_fileContentsBuffer, fileContents, bufferSize );
			out_fileContentsBuffer[ bufferSize-1 ] = '\0';

			free( fileContents );

			return true;
		}
		else
		{
			return false;
		}
	}

}