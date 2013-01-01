//David Oguns
//February 13, 2011
//XmlStreamLogger.h

#ifndef _XMLSTREAMLOGGER_H_
#define _XMLSTREAMLOGGER_H_

#include "BaseLogger.h"
//#include <ostream>
#include <fstream>

namespace Accevo
{
	class XmlStreamLogger : public BaseLogger
	{
	public:
		//constructs an xml file logger where this instance holds the root of the
		//xml stream
		XmlStreamLogger(char const *filename, char const *name, Clock const & clock, LogLevel level);
		//constructs an xml file logger where this instance simply uses a stream
		//already opened by the creator
		XmlStreamLogger(std::wostream &wostr, char const *name, Clock const & clock, LogLevel level);
		//destructs XML file logger destructing the underlying stream if this instance
		//owns it.  if not, the underlying output stream is left alone
		~XmlStreamLogger();

		//Log level management done by BaseLogger
		virtual void SetLogLevel(LogLevel level);

		//Logs a message with a character string message
		virtual void LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, char const *message);
		//Logs a message with a wide character string message
		virtual void LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, wchar_t const *message);

	protected:
		//creates the opening tag for xml log messages
		void LogMessageXmlStart(LogLevel level, unsigned int line, char const *file, char const *function);

		std::wofstream					m_wofstream;
		std::wostream &					m_wostream;
	};
};

#endif