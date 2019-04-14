#pragma once
#include <string>
#include <vector>
#include <atomic>

/*
	Debug is a singleton class which you can use to print logs in file or on console string.
	Log method has a default parameter which decides where to put logs. By default it is set to false.
	if flag is false -> print to file
	else if flag is true ->print to console
	Usage:
		To print logs on screen,
		Debug::Log("log statement",true);

		To print in the file
		Debug::Log("log statement");
*/


class Debug
{
private:
	std::string logFilePath;
	std::vector<std::string> logs;
	static std::atomic<int> index;
	Debug();
	~Debug();
	void AppendLog(const std::string & logLine);
	void DumpToFile();
	void DumpToConsole(const std::string &logline);
public:
	static void Log(const std::string & logLine,bool dumpToConsole=false);
};

