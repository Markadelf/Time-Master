#pragma once
#include <string>
#include <vector>
#include <atomic>

class Debug
{
private:
	std::string logFilePath;
	std::vector<std::string> logs;
	static std::atomic<unsigned long> index;
	Debug();
	~Debug();
	void AppendLog(const std::string & logLine);
	void DumpToFile();
	void DumpToConsole(const std::string &logline);
public:
	static void Log(const std::string & logLine,bool dumpToConsole=false);
};

