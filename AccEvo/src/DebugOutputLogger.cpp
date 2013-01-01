//David Oguns
//February 13, 2011

#include "DebugOutputLogger.h"
#include <sstream>
#include <string>

namespace Accevo
{
	//creates a logger that logs to the visual studio debug window
	//using OutputDebugString();
	DebugOutputLogger::DebugOutputLogger(char const *name, Clock const & clock, LogLevel level) :
		BaseLogger(name, clock, level)
	{

	}

	DebugOutputLogger::~DebugOutputLogger()
	{

	}

	//Log message with char string
	void DebugOutputLogger::LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, char const *message)
	{
		std::wostringstream out;
		out << "t = <" << Clock::CyclesToSeconds(m_clock.GetTimeCycles()) << "> [" 
			<< function << "] (" << file << ") {" << line << "} "
			<< Logger::GetLogLevelString(level) << " -- " << message << std::endl;
		OutputDebugString(out.str().c_str());
	}

	//Log message with wchar_t string
	void DebugOutputLogger::LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, wchar_t const *message)
	{
		std::wostringstream out;
		out << "t = <" << Clock::CyclesToSeconds(m_clock.GetTimeCycles()) << "> [" 
			<< function << "] (" << file << ") {" << line << "} "
			<< Logger::GetLogLevelString(level) << " -- " << message << std::endl;
		OutputDebugString(out.str().c_str());
	}
}