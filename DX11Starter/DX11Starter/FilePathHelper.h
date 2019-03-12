#pragma once

#include<string>

// Simple Class that handles file path operations,
// May add complexity later, but for now I just wanted a place for the logic
class FilePathHelper
{
private:
	static std::string ASSETPATH;
	static std::wstring ASSETPATHW;
	FilePathHelper();
	~FilePathHelper();

public:
	static std::string GetPath(std::string filename);
	static std::wstring GetPath(std::wstring filename);
};

