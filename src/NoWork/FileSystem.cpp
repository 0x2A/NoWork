#include <NoWork/FileSystem.h>
#include <NoWork/Log.h>
#include <fstream>
#include <streambuf>

const char* FileSystem::GetPath(const char* path)
{
	std::string fName(path);
	size_t pos = fName.rfind("/");
	if (pos == std::string::npos)
	{
		pos = fName.rfind("\\");
		if (pos == std::string::npos)
			return fName.c_str();
	}

	if (pos == 0)    // / is at the front.
		return fName.c_str();

	return fName.substr(0, pos).c_str();
}

const char* FileSystem::GetFilename(const char* path)
{
	std::string fName(path);
	size_t pos = fName.rfind("/");
	if (pos == std::string::npos)
	{
		pos = fName.rfind("\\");
		if (pos == std::string::npos)
			return fName.c_str();
	}

	if (pos == 0)    // / is at the front.
		return fName.c_str();

	return fName.substr(pos + 1, fName.size() - (pos + 1)).c_str();
}

const char* FileSystem::LoadTextFile(const char* path)
{
	std::ifstream t(path, std::ios::in);
	if (!t.is_open())
	{
		std::stringstream ss;
		ss << "Unable to open file '" << path << "': " << std::strerror(errno);
		throw std::exception(ss.str().c_str());
	}

	std::string src((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	t.close();
	return src.c_str();
}

