#pragma once

#include <NoWork/Common.h>


class FileSystem
{
public:

	NOWORK_API static std::string GetPath(const const char* path);
	NOWORK_API static std::string GetFilename(const const char* path);
	NOWORK_API static std::string LoadTextFile(const const char* path);

};