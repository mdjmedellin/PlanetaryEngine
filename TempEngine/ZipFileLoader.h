#pragma once
#ifndef	ZIP_FILE_LOADER_H
#define ZIP_FILE_LOADER_H

//====================================INCLUDE==============================================
#include "FileLoader.h"
#include "ZipFile.h"
//=========================================================================================

namespace gh
{
	class ZipFileLoader : public FileLoader
	{
	public:
		ZipFileLoader( const char* filePath, const char* password = nullptr );
		virtual bool loadFile( const char* filePath, char*& out_fileBuffer, int& out_bufferSize );

	private:
		ZipFile* m_zipFolder;
		const char* m_password;
	};
}

#endif