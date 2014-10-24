#pragma once
#ifndef DISK_FILE_LOADER_H
#define DISK_FILE_LOADER_H

//===============================INCLUDES========================================
#include "FileLoader.h"
#include <string>
//===============================================================================

namespace gh
{
	class DiskFileLoader : public FileLoader
	{
	public:
		DiskFileLoader( const std::string& diskDataFolderPath );
		virtual bool loadFile( const char* filePath, char*& out_fileBuffer, int& bufferSize );

	private:
		std::string m_diskToDataFolder;
	};
}

#endif