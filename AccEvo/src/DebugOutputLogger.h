//David Oguns
//DebugOutputLogger.h
//February 13, 2011

#ifndef _DEBUGOUTPUTLOGGER_H_
#define _DEBUGOUTPUTLOGGER_H_

#include "BaseLogger.h"

namespace Accevo
{
	class DebugOutputLogger : public BaseLogger
	{
	public:
		//creates a logger that logs to the visual studio debug window
		//using OutputDebugString();
		DebugOutputLogger(char const *name, Clock const & clock, LogLevel level);
		~DebugOutputLogger();

		//Log message with char string
		virtual void LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, char const *message);
		//Log message with wchar_t string
		virtual void LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, wchar_t const *message);

	protected:
	};
}

#endif