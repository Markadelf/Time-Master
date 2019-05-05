#include "FilePathHelper.h"

// Use two so one can handle Wide characters
#ifdef DEBUG
std::string FilePathHelper::ASSETPATH = "../Release/Assets/";
std::wstring FilePathHelper::ASSETPATHW = L"../Release/Assets/";
#endif // DEBUG

#ifndef DEBUG
std::string FilePathHelper::ASSETPATH = "Assets/";
std::wstring FilePathHelper::ASSETPATHW = L"Assets/";
#endif // !DEBUG

FilePathHelper::FilePathHelper()
{
}


FilePathHelper::~FilePathHelper()
{
}

std::string FilePathHelper::GetPath(std::string filename)
{
	return ASSETPATH + filename;
}

std::wstring FilePathHelper::GetPath(std::wstring filename)
{
	return ASSETPATHW + filename;
}
