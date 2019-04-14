// INCLUDES /////////////////////////////////////////////////////////////////////////////

// windows
//#include <Windows.h>
//#include <KnownFolders.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Pathcch.h>

// bell0bytes
#include "log.h"

// DLLs /////////////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

namespace util
{
	// FUNCTIONS ////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////// File Logger /////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	// the openOutputStream function opens a file on the hard drive and sets the output precision
	bool FileLogPolicy::openOutputStream(const std::wstring& filename)
	{
		// try to open the file
		outputStream.open(filename.c_str(), std::ios_base::binary | std::ios_base::out);

		// debug mode only: make sure the file is opened
#ifndef NDEBUG
		if (!outputStream.is_open())
			return false;
#endif
		// set output precision
		outputStream.precision(20);

		// return success
		return true;
	}

	// the closeOutputStream function closes the file on the hard drive
	void FileLogPolicy::closeOutputStream()
	{
		outputStream.close();
	}

	// the write functions writes a message :)
	void FileLogPolicy::write(const std::string& msg)
	{
		// add the message to the stream
		outputStream << msg << std::endl;
	}
}