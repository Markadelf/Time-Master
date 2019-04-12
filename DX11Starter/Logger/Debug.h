#pragma once
#include <string>
#include <condition_variable>

class Debug
{
private:
	std::string logFilePath;
	std::string logs;
	std::mutex thread_mutex;
	Debug();
	~Debug();
	void AppendLog(const std::string & logLine);
public:
	static void Log(const std::string & logLine, bool isThreadSafe = false);
};

