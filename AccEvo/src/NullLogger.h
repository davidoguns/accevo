//David Oguns
//NullLogger.h
//February 13, 2011

#ifndef _NULLLOGGER_H_
#define _NULLLOGGER_H_

#include "Logger.h"

namespace Accevo
{
	/**************************************************
		Does nothing for all log calls.
	**************************************************/

	class NullLogger : public Logger
	{
	public:
		//WARN: not multithread safe
		static Logger * GetInstance();

	protected:
		NullLogger();
		~NullLogger();

	public:

		virtual LogLevel GetLogLevel() { return Logger::AELOGLEVEL_OFF; }
		virtual void SetLogLevel(LogLevel level) { }
	
		virtual void LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, char const *message) { };
		virtual void LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, wchar_t const *message) { };

	protected:
		static NullLogger		theNullLogger;	//To rule them all
	};
}

#endif