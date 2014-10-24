#pragma once
#ifndef ZIP_FILE_MANAGER_H
#define ZIP_FILE_MANAGER_H

namespace gh
{
	class ZipFile;

	class ZipFileManager
	{
	public:
		ZipFile* getZipFile( const char* zipFileName );
	};
}

extern gh::ZipFileManager theZipFileManager;

#endif