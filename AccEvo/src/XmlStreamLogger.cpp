//David Oguns
//February 13, 2011
//XmlStreamLogger.cpp

#include "XmlStreamLogger.h"

namespace Accevo
{
	//output character at a time replacing only taboo ones
	void WriteStringToXmlStream(char const *string, std::wostream &os)
	{
		while(*string != '\0')
		{
			switch(*string)
			{
			case '"':
				os << "&quot;";
				break;
			case '\'':
				os << "&apos;";
				break;
			case '<':
				os << "&lt;";
				break;
			case '>':
				os << "&gt;";
				break;
			case '&':
				os << "&amp;";
				break;
			default:
				os << *string;
			}
			++string;
		}
	}

	//output character at a time replacing only taboo ones
	void WriteStringToXmlStream(wchar_t const *string, std::wostream &os)
	{
		while(*string != '\0')
		{
			switch(*string)
			{
			case '"':
				os << "&quot;";
				break;
			case '\'':
				os << "&apos;";
				break;
			case '<':
				os << "&lt;";
				break;
			case '>':
				os << "&gt;";
				break;
			case '&':
				os << "&amp;";
				break;
			default:
				os << *string;
			}
			++string;
		}
	}
	
	char const *				AE_XML_LOG_START = "<Log";
	char const *				AE_XML_LOG_END = "</Log>";
	char const *				AE_XML_LOG_MESSAGE_START = "<LogMessage";	//attributes must be filled in before closing angle bracket
	char const *				AE_XML_LOG_MESSAGE_END = "</LogMessage>";


	//constructs an xml file logger where this instance holds the root of the
	//xml stream
	XmlStreamLogger::XmlStreamLogger(char const *filename, char const *name, Clock const & clock, LogLevel level) :
		BaseLogger(name, clock, level),
		m_wofstream(filename, std::ios_base::out),
		m_wostream(m_wofstream)
	{
		if(m_wofstream)
		{
			m_wostream << AE_XML_LOG_START << " name=\"";
			WriteStringToXmlStream(m_pName, m_wostream);
			m_wostream << "\" level=\"" << Logger::GetLogLevelString(level)
				<< "\">" << std::flush;
		}
	}

	//constructs an xml file logger where this instance simply uses a stream
	//already opened by the creator
	XmlStreamLogger::XmlStreamLogger(std::wostream &wostr, char const *name, Clock const & clock, LogLevel level) :
		BaseLogger(name, clock, level),
		m_wostream(wostr)
	{

	}

	//destructs XML file logger destructing the underlying stream if this instance
	//owns it.  if not, the underlying output stream is left alone
	XmlStreamLogger::~XmlStreamLogger()
	{
		if(m_wofstream)
		{
			m_wostream << AE_XML_LOG_END;
			m_wofstream.close();
		}
	}

	//creates the opening tag for xml log messages
	void XmlStreamLogger::LogMessageXmlStart(LogLevel level, unsigned int line, char const *file, char const *function)
	{
		m_wostream << AE_XML_LOG_MESSAGE_START 
				<< " name=\"";
		WriteStringToXmlStream(m_pName, m_wostream);
		m_wostream << "\" level=\"" << GetLogLevelString(level)
			<< "\" time=\"" << Clock::CyclesToSeconds(m_clock.GetTimeCycles())
			<< "\" file=\"";
		WriteStringToXmlStream(file, m_wostream);
		m_wostream << "\" line=\"" << line;
		m_wostream << "\" function=\"";
		WriteStringToXmlStream(function, m_wostream);
		m_wostream << "()\">";
	}

	//Logs a message with a character string message
	void XmlStreamLogger::LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, char const *message)
	{
		//only log messages appropriate to the level of this logger
		if(level >= m_logLevel)
		{
			LogMessageXmlStart(level, line, file, function);
			WriteStringToXmlStream(message, m_wostream);
			m_wostream << AE_XML_LOG_MESSAGE_END << std::flush;
		}
	}

	//Logs a message with a wide character string message
	void XmlStreamLogger::LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, wchar_t const *message)
	{
		//only log messages appropriate to the level of this logger
		if(level >= m_logLevel)
		{
			LogMessageXmlStart(level, line, file, function);
			WriteStringToXmlStream(message, m_wostream);
			m_wostream << AE_XML_LOG_MESSAGE_END << std::flush;
		}
	}

	void XmlStreamLogger::SetLogLevel(LogLevel level)
	{
		m_wostream << AE_XML_LOG_MESSAGE_START << ">Changing log level to: " << GetLogLevelString(level) 
			<< AE_XML_LOG_MESSAGE_END;
		m_logLevel = level;

	}

}