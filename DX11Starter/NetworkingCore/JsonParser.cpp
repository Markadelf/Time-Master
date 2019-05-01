#include "JsonParser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

JsonParser::JsonParser(const char* file_path)
{
	std::string line;
	std::string key;
	std::string val;
	std::ifstream jsonFile(file_path);
	std::string delimeter = ":";
	size_t index;
	if (jsonFile.is_open())
	{
		while (getline(jsonFile, line))
		{
			if (line[0] == '{' || line[0] == '}')
				continue;
			// trimming spaces
			line = std::regex_replace(line, std::regex("^ +| +$|( ) +"), "$1");
			//splitting line
			index = line.find(delimeter);
			if (index == std::string::npos)
			{
				std::cout << "Corrupted JSON file" << std::endl;
			}

			//pushing to the map
			
			key = line.substr(0, index);
			val = line.substr(index + 1, line.size());
			key.erase(0, 1);
			key.erase(key.size() - 1);
			val.erase(0, 1);
			val.erase(val.size() - 1);
			jsonData[key] = val;
		}
	}
}



JsonParser::~JsonParser()
{
}

void JsonParser::GetIpAddr(int * ipAddr)
{
	std::string delimeter = ".";
	if (jsonData.find("ip") != jsonData.end())
	{
		std::string val = jsonData["ip"];
		size_t index = 0;
		int ip_index = 0;
		while (val.size() > 0 && index != std::string::npos)
		{
			index = val.find(delimeter);
			if (index == std::string::npos)
				ipAddr[ip_index] = std::atoi((val).c_str());
			else
			{
				ipAddr[ip_index] = std::atoi((val.substr(0, index)).c_str());
				val.erase(0, index + 1);
			}
			//std::cout << "Inside method" << ipAddr[ip_index] << std::endl;
			ip_index++;
		}
	}
	else
	{
		std::cout << "IP not specified in json" << std::endl;
	}
}
