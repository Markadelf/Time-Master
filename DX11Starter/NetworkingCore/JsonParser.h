#pragma once
#include <map>
class JsonParser
{
private:
	std::map<std::string,std::string> jsonData;

public:
	JsonParser(const char* file_path);
	~JsonParser();
	void GetIpAddr(int *ipAddr);
};

