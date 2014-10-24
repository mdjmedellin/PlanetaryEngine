#include "FileManager.h"
#include "DiskFileLoader.h"
#include "ZipFileLoader.h"

#include "MemoryUtilities.h"

namespace gh
{
	FileManager::~FileManager()
	{
		if( m_diskFileLoader )
		{
			delete m_diskFileLoader;
		}
		if( m_zipFileLoader )
		{
			delete m_zipFileLoader;
		}
	}

	FileManager::FileManager()
		:	m_diskFileLoader( nullptr )
		,	m_zipFileLoader( nullptr )
	{}

	FileManager& FileManager::getSingleton()
	{
		static FileManager s_FileManager;
		return s_FileManager;
	}

	void FileManager::setDiskDataLocation( const char* dataFolderDiskLocation )
	{
		if( !m_diskFileLoader )
		{
			m_diskFileLoader = new DiskFileLoader( std::string( dataFolderDiskLocation ).c_str() );
		}
	}

	bool FileManager::loadFile( const char* filePath, char*& out_fileBuffer, int& out_bufferSize )
	{
		return getSingleton().private_LoadFile( filePath, out_fileBuffer, out_bufferSize );
	}

	bool FileManager::private_LoadFile( const char* filePath, char*& out_fileBuffer, int& out_bufferSize )
	{
		bool retVal = false;

		switch( m_loadingMode )
		{
		case DATA_FOLDER_ONLY:
			{
				if( m_diskFileLoader )
				{
					return m_diskFileLoader->loadFile( filePath, out_fileBuffer, out_bufferSize );
				}
				else
				{
					return false;
				}
			}
		case ZIP_DATA_FOLDER_ONLY:
			{
				if( m_zipFileLoader )
				{
					return m_zipFileLoader->loadFile( filePath, out_fileBuffer, out_bufferSize );
				}
				else
				{
					return false;
				}
			}
		case PREFER_DATA:
			{
				if( m_diskFileLoader )
				{
					retVal = m_diskFileLoader->loadFile( filePath, out_fileBuffer, out_bufferSize );
				}
				if( !retVal && m_zipFileLoader )
				{
					retVal = m_zipFileLoader->loadFile( filePath, out_fileBuffer, out_bufferSize );
				}

				return retVal;
			}
		default:
			{
				if( m_zipFileLoader )
				{
					retVal = m_zipFileLoader->loadFile( filePath, out_fileBuffer, out_bufferSize );
				}
				if( !retVal && m_diskFileLoader )
				{
					retVal = m_diskFileLoader->loadFile( filePath, out_fileBuffer, out_bufferSize );
				}

				return retVal;
			}
		}
	}

	void FileManager::setLoadingMode( FileLoadingMode loadingMode )
	{
		m_loadingMode = loadingMode;
	}

	void FileManager::setArchiveLocation( const char* archiveLocation, const char* password /*= nullptr*/ )
	{
		if( !m_zipFileLoader )
		{
			m_zipFileLoader = new ZipFileLoader( archiveLocation, password );
		}
	}

}