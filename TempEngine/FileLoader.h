#pragma once
#ifndef FILE_LOADER_H
#define FILE_LOADER_H

namespace gh
{
	class FileLoader
	{
	public:
		virtual bool loadFile( const char* filePath, char*& out_fileBuffer );

	private:
	};
}

#endif