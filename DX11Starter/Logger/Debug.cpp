#include <iostream>
#include <fstream>
#include <chrono>

#include "Debug.h"

Debug::Debug()
	:logFilePath("logs.txt"),logs("")
{
	
}


Debug::~Debug()
{
	std::ofstream logfile;
	logfile.open(logFilePath, std::ios::out | std::ios::trunc);
	if (logfile.is_open())
	{
		logfile << logs;
		logfile.close();
	}
	else
	{
		std::cout << "Failed to open logFiles, dumping code on the console." << std::endl;
		std::cout << logs << std::endl;
		std::cin >> logs;
	}
}

void Debug::AppendLog(const std::string & logLine)
{
	logs.append(logLine);
	logs.append("\n");
}

void Debug::Log(const std::string & logLine, bool isThreadSafe = false)
{
	static Debug debug;
	if (isThreadSafe)
	{
		std::lock()
	}
	debug.AppendLog(logLine);
}
