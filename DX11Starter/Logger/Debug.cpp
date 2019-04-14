#include <iostream>
#include <stdio.h>
#include <fstream>
#include <time.h>
#include <climits>

#include "Debug.h"

std::atomic<int> Debug::index = -1;

Debug::Debug()
	:logFilePath("logs.txt")
{
	index = -1;
	logs.resize(SHRT_MAX);
}

void Debug::DumpToFile()
{
	std::ofstream logfile;
	logfile.open(logFilePath, std::ios::out | std::ios::trunc);
	if (logfile.is_open())
	{
		char buff[20];
		struct tm *sTm;
		time_t now = time(0);

		for (int i = 0; i <= index; i++)
		{
			sTm = gmtime(&now);
			strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
			logfile << buff << " : " << logs[i];
			logfile << "\n";
		}
		logfile.close();
	}
	else
	{
		std::cout << "Failed to open logFiles, dumping code on the console." << std::endl;
		//std::cout << logs << std::endl;
		//std::cin >> logs;
	}
}
void Debug::DumpToConsole(const std::string & logline)
{
	char buff[20];
	struct tm *sTm;
	time_t now = time(0);
	sTm = gmtime(&now);
	strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
	printf("%s : %s\n",buff, logline.c_str());
}
Debug::~Debug()
{
	if(index>=0)
		this->DumpToFile();
}

void Debug::AppendLog(const std::string & logLine)
{
	++index;
	logs[index] = logLine;
}

void Debug::Log(const std::string & logLine,bool dumpToConsole)
{
	static Debug debug;
	if (!dumpToConsole)
	{
		debug.AppendLog(logLine);
	}
	else
	{
		debug.DumpToConsole(logLine);
	}
}
