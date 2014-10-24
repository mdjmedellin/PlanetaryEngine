#include "ZipFileManager.h"
#include "ZipFile.h"

#include "MemoryManager.h"

gh::ZipFileManager theZipFileManager;

namespace gh
{
	ZipFile* ZipFileManager::getZipFile( const char* zipFileName )
	{
		ZipFile* newlyCreatedZipFile = new ZipFile( zipFileName );
		if( newlyCreatedZipFile->isGood() )
		{
			return newlyCreatedZipFile;
		}
		else
		{
			delete newlyCreatedZipFile;
			return NULL;
		}
	}
}