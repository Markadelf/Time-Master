#pragma once

/****************************************************************************************
* Author:	Gilles Bellot
* Date:		13/09/2016 - Lenningen - Luxembourg
*
* Desc:		service locator
*
* History:	- 07/10/2016: added file logging service
*
****************************************************************************************/

// INCLUDES /////////////////////////////////////////////////////////////////////////////

// bell0bytes
#include "log.h"							// logging

namespace util
{
	// CLASSES //////////////////////////////////////////////////////////////////////////////

	class ServiceLocator
	{
	private:
		static std::shared_ptr<Logger<FileLogPolicy> > fileLogger;						// the file logger
		
	public:
		// file logging services
		static Logger<FileLogPolicy>* getFileLogger() { return fileLogger.get(); };									// returns the file logger
		static void provideFileLoggingService(const std::shared_ptr<Logger<FileLogPolicy> > providedFileLogger);	// sets the file logging service to the given logger
	};
}