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

void Debug::GetTimeStamp(char * buff,int size)
{
	struct tm sTm;
	time_t now = time(0);
	localtime_s(&sTm, &now);
	strftime(buff, size, "%Y-%m-%d %H:%M:%S", &sTm);
}

void Debug::DumpToFile()
{
	std::ofstream logfile;
	logfile.open(logFilePath, std::ios::out | std::ios::trunc);
	if (logfile.is_open())
	{
		char buff[20];
		for (int i = 0; i <= index; i++)
		{
			GetTimeStamp(buff, sizeof(buff));
			logfile << buff << " : " << logs[i];
			logfile << "\n";
		}
		logfile.close();
	}
	else
	{
		printf("Failed to open log file, dumping code on the console.\n");
	}
}
void Debug::DumpToConsole(const std::string & logline)
{
	char buff[20];
	GetTimeStamp(buff,sizeof(buff));
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
