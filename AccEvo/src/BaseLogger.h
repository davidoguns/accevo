//David Oguns
//BaseLogger.h
//February 13, 2011

#ifndef _BASELOGGER_H_
#define _BASELOGGER_H_

#include "Logger.h"
#include "Clock.h"

namespace Accevo
{
	class BaseLogger : public Logger
	{
	public:
		//simple holds the basics to start logging information
		BaseLogger(char const *name, Clock const & clock, LogLevel level) :
			m_pName(name),
			m_clock(clock),
			m_logLevel(level) {}
		//Doesn't really do much.  no memory cleanup needed
		~BaseLogger() { };

		//Gets the log level
		virtual LogLevel GetLogLevel() { return m_logLevel; }
		//changes the log level
		virtual void SetLogLevel(LogLevel level) { m_logLevel = level; }

	protected:
		char const *				m_pName;
		Clock const &				m_clock;
		LogLevel					m_logLevel;
	};
}

#endif