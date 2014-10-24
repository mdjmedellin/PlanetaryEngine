#pragma once
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

enum FileLoadingMode{
	DATA_FOLDER_ONLY,
	ZIP_DATA_FOLDER_ONLY,
	PREFER_DATA,
	PREFER_ZIP
};

//==========================================INCLUDES=========================================
#include <string>
//===========================================================================================

namespace gh
{
	//======================================FORWARD DECLARATION====================================
	class DiskFileLoader;
	class ZipFileLoader;
	//=============================================================================================

	class FileManager
	{
	public:
		//======================================STATIC INTERFACE===============================================
		static FileManager& getSingleton();
		static bool loadFile( const char* filePath, char*& out_fileBuffer, int& out_bufferSize );
		//=====================================================================================================

		void setDiskDataLocation( const char* dataFolderDiskLocation );
		void setArchiveLocation( const char* archiveLocation, const char* password = nullptr );
		void setLoadingMode( FileLoadingMode loadingMode );

		~FileManager();

	private:
		FileManager();
		FileManager( const FileManager& other );
		FileManager& operator=(const FileManager& other );

		bool private_LoadFile( const char* filePath, char*& out_fileBuffer, int& out_bufferSize );

		DiskFileLoader* m_diskFileLoader;
		ZipFileLoader* m_zipFileLoader;
		FileLoadingMode m_loadingMode;
	};
}

#endif