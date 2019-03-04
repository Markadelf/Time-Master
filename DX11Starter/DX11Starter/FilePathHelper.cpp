#include "FilePathHelper.h"

// Use two so one can handle Wide characters
std::string FilePathHelper::ASSETPATH =		 "../../Assets/";
std::wstring FilePathHelper::ASSETPATHW =	L"../../Assets/";

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
