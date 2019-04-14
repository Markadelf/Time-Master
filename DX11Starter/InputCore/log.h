#pragma once

/****************************************************************************************
* Author:	Gilles Bellot
* Date:		13/09/2016 - Lenningen - Luxembourg
*
* Desc:		Event Logger
*
* History:	- 01/07/2017: fixed a memory leak
*			- 02/07/2017: added overloaded print function to take a string
*
****************************************************************************************/

// INCLUDES /////////////////////////////////////////////////////////////////////////////

// windows includes
#include <Windows.h>						// standard Windows stuff

// c++ includes
#include <atomic>							// atomic objects (no data races) 
#include <thread>							// individual threats
#include <mutex>							// lockable objects
#include <iostream>							// input and output streams
#include <sstream>							// string streams
#include <fstream>							// file streams
#include <vector>							// vector containers
#include <map>								// map containers


namespace util
{
	// CLASSES //////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////// LOG POLICIES /////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////

	// virtual abstract class - interface to open and close streams (and to write to them)
	class LogPolicyInterface
	{
	public:
		virtual ~LogPolicyInterface() noexcept = default;

		virtual bool openOutputStream(const std::wstring& name) = 0;
		virtual void closeOutputStream() = 0;
		virtual void write(const std::string& msg) = 0;
	};

	// implementation of a policy to write to a file on the hard drive
	class FileLogPolicy : public LogPolicyInterface
	{
	private:
		std::ofstream outputStream;

	public:
		FileLogPolicy() : outputStream() {};
		~FileLogPolicy() { };

		bool openOutputStream(const std::wstring& filename) override;
		void closeOutputStream() override;
		void write(const std::string& msg) override;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////// MESSAGE TYPES ////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////

	enum SeverityType
	{
		info = 0,
		debug,
		warning,
		error,
		config
	};

	/////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////// LOGGER ///////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////

	// define the class
	template<typename LogPolicy>
	class Logger;

	// create the actual logging daemon
	template<typename LogPolicy>
	void loggingDaemon(Logger<LogPolicy>* logger)
	{
		// dump log data if present
		std::unique_lock<std::timed_mutex> lock(logger->writeMutex, std::defer_lock);
		do
		{
			std::this_thread::sleep_for(std::chrono::milliseconds{ 50 });
			if (logger->logBuffer.size())
			{
				if (!lock.try_lock_for(std::chrono::milliseconds{ 50 }))
					continue;
				for (auto& x : logger->logBuffer)
					logger->policy.write(x);
				logger->logBuffer.clear();
				lock.unlock();
			}
		} while (logger->isStillRunning.test_and_set() || logger->logBuffer.size());
	}

	// the actual logger class to be instantiated with a specific log policy
	template<typename LogPolicy>
	class Logger
	{
	private:
		unsigned int logLineNumber;								// used to save the current line number
		std::map<std::thread::id, std::string> threadName;		// defines a human-readable name for each thread
		LogPolicy policy;										// the log policy (i.e. write to file, ...)
		std::timed_mutex writeMutex;							// mutual exclusive writer
		std::vector<std::string> logBuffer;						// the content to log
		std::thread daemon;										// the actual logging daemon
		std::atomic_flag isStillRunning{ ATOMIC_FLAG_INIT };	// lock-free boolean to check whether our daemon is still running or not

	public:
		// constructor and destructor
		Logger(const std::wstring& name);
		~Logger();

		void setThreadName(const std::string& name);			// sets human-readable name for current thread

		template<SeverityType severity>
		void print(std::stringstream stream);					// print message (varies based on severity level)
		template<SeverityType severity>
		void print(std::string msg);

		template<typename Policy>
		friend void loggingDaemon(Logger<Policy>* logger);		// the actual logger daemon
	};

	template<typename LogPolicy>
	Logger<LogPolicy>::Logger(const std::wstring& name) : logLineNumber(0), threadName(), policy(), writeMutex(), logBuffer()
	{
		if (policy.openOutputStream(name))
		{
			isStillRunning.test_and_set();						// mark the logging daemon as running
			daemon = std::move(std::thread{ loggingDaemon<LogPolicy>, this });
		}
		else
			throw std::runtime_error("Unable to open the log file!");
	}

	template<typename LogPolicy>
	Logger<LogPolicy>::~Logger()
	{
#ifndef NDEBUG
		// print closing message
		util::ServiceLocator::getFileLogger()->print<util::SeverityType::info>("The file logger was destroyed.");
#endif
		// terminate the daemon by clearing the still running flag and letting it join to the main thread
		isStillRunning.clear();
		daemon.join();

		// clear the thread name map
		threadName.clear();
		std::map<std::thread::id, std::string>().swap(threadName);

		// clear the log vector
		logBuffer.clear();
		logBuffer.shrink_to_fit();

		// close the output stream
		policy.closeOutputStream();
	}

	template<typename LogPolicy>
	void Logger<LogPolicy>::setThreadName(const std::string& name)
	{
		threadName[std::this_thread::get_id()] = name;
	}

	template<typename LogPolicy>
	template<SeverityType severity>
	void Logger<LogPolicy>::print(std::stringstream stream)
	{
		std::stringstream logStream;
#pragma warning (push)
#pragma warning (disable: 4127)		// disable constant if expr warning
		// all severity types but the config type allow custom formatting
		if(!(severity == SeverityType::config))		// todo: eliminate this warning with if constexpr once version 15.3 of Visual Studio with C++-17 support is available
#pragma warning (pop)
		{
			// get time
			SYSTEMTIME localTime;
			GetLocalTime(&localTime);

			// header: line number and date (x: xx/xx/xxxx xx:xx:xx)
			if (logLineNumber != 0)
				logStream << "\r\n";
			logStream << logLineNumber++ << ": " << localTime.wDay << "/" << localTime.wMonth << "/" << localTime.wYear << " " << localTime.wHour << ":" << localTime.wMinute << ":" << localTime.wSecond << "\t";

			// write down warning level
			switch (severity)
			{
			case SeverityType::info:
				logStream << "INFO:    ";
				break;
			case SeverityType::debug:
				logStream << "DEBUG:   ";
				break;
			case SeverityType::warning:
				logStream << "WARNING: ";
				break;
			case SeverityType::error:
				logStream << "ERROR:   ";
				break;
			};

			// write thread name
			logStream << threadName[std::this_thread::get_id()] << ":\t";
		}
		
		// write the actual message
		logStream << stream.str();
		std::lock_guard<std::timed_mutex> lock(writeMutex);
		logBuffer.push_back(logStream.str());
	}

	template<typename LogPolicy>
	template<SeverityType severity>
	void Logger<LogPolicy>::print(std::string msg)
	{
		std::stringstream stream;
		stream << msg.c_str();
		this->print<severity>(std::stringstream(stream.str()));
	}
}