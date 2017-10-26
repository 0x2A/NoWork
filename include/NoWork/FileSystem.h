#pragma once

#include <NoWork/Common.h>


class FileSystem
{
public:

	NOWORK_API static const char* GetPath(const const char* path);
	NOWORK_API static const char* GetFilename(const const char* path);
	NOWORK_API static const char* LoadTextFile(const const char* path);

};