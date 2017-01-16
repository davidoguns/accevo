//David Oguns
//January 18th, 2011
//Logger.h

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <ostream>
#include "Clock.h"

namespace Accevo
{
	//TODO:  Add virtual destructor to ensure derived class destructors are called properly
	//		when Logger abstraction is used.
	class Logger
	{
	public:
		/**
		  * Level of log message
		  */
		enum LogLevel
		{
			AELOGLEVEL_TRACE,
			AELOGLEVEL_DEBUG,
			AELOGLEVEL_INFO,
			AELOGLEVEL_WARN,
			AELOGLEVEL_ERROR,
			AELOGLEVEL_FATAL,
			AELOGLEVEL_OFF	//technically just another level
		};

		virtual LogLevel GetLogLevel() = 0;
		virtual void SetLogLevel(LogLevel level) = 0;
		
		//Logs a message with a character string message
		virtual void LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, char const *message) = 0;
		//Logs a message with a wide character string message
		virtual void LogMessage(LogLevel level, unsigned int line, char const *file, char const *function, wchar_t const *message) = 0;			
	protected:
		//Returns the string representing the log level passed in
		static char const * GetLogLevelString(Logger::LogLevel level)
		{
			switch(level)
			{
			case Logger::AELOGLEVEL_TRACE:
				return "TRACE";
			case Logger::AELOGLEVEL_DEBUG:
				return "DEBUG";
			case Logger::AELOGLEVEL_INFO:
				return "INFO";
			case Logger::AELOGLEVEL_WARN:
				return "WARN";
			case Logger::AELOGLEVEL_ERROR:
				return "ERROR";
			case Logger::AELOGLEVEL_FATAL:
				return "FATAL";
			case Logger::AELOGLEVEL_OFF:
				return "OFF";
			default:
				return "UNKNOWN";
			}
		}
	};
}

#define AE_LOG_OUTPUT_TRACE				6
#define AE_LOG_OUTPUT_DEBUG				5
#define AE_LOG_OUTPUT_INFO				4
#define AE_LOG_OUTPUT_WARN				3
#define AE_LOG_OUTPUT_ERROR				2
#define AE_LOG_OUTPUT_FATAL				1
#define AE_LOG_OUTPUT_OFF				0

//default output level is 
#if(!defined(AE_LOG_OUTPUT_LEVEL))
	#if(defined(_DEBUG) || defined(DEBUG))
		#define AE_LOG_OUTPUT_LEVEL AE_LOG_OUTPUT_TRACE
	#else
		#define AE_LOG_OUTPUT_LEVEL AE_LOG_OUTPUT_INFO
	#endif
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_TRACE))
	//Log a trace-level message
	#define AELOG_TRACE(pLogger, message) AELOG_MESSAGE(pLogger, Accevo::Logger::AELOGLEVEL_TRACE, message)
	#define AELOG_CONDITIONAL_TRACE(pLogger, message, cond)				\
	{																	\
		bool _aeEvalBool = static_cast<bool>(cond);						\
		if(_aeEvalBool)													\
		{																\
			AELOG_TRACE(pLogger, message);								\
		}																\
	}
#else
	#define AELOG_TRACE(pLogger, message)
	#define AELOG_CONDITIONAL_TRACE(pLogger, message, cond)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_DEBUG))
	//Log a debug-level message
	#define AELOG_DEBUG(pLogger, message) AELOG_MESSAGE(pLogger, Accevo::Logger::AELOGLEVEL_DEBUG, message)
	#define AELOG_CONDITIONAL_DEBUG(pLogger, message, cond)				\
	{																	\
		bool _aeEvalBool = static_cast<bool>(cond);						\
		if(_aeEvalBool)													\
		{																\
			AELOG_DEBUG(pLogger, message);								\
		}																\
	}
#else
	#define AELOG_DEBUG(pLogger, message)
	#define AELOG_CONDITIONAL_DEBUG(pLogger, message, cond)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_INFO))
	//Log an info-level message
	#define AELOG_INFO(pLogger, message) AELOG_MESSAGE(pLogger, Accevo::Logger::AELOGLEVEL_INFO, message)
	#define AELOG_CONDITIONAL_INFO(pLogger, message, cond)				\
	{																	\
		bool _aeEvalBool = static_cast<bool>(cond);						\
		if(_aeEvalBool)													\
		{																\
			AELOG_INFO(pLogger, message);								\
		}																\
	}
#else
	#define AELOG_INFO(pLogger, message)
	#define AELOG_CONDITIONAL_INFO(pLogger, message, cond)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_WARN))
	//Log a warn-level message
	#define AELOG_WARN(pLogger, message) AELOG_MESSAGE(pLogger, Accevo::Logger::AELOGLEVEL_WARN, message)
	#define AELOG_CONDITIONAL_WARN(pLogger, message, cond)				\
	{																	\
		bool _aeEvalBool = static_cast<bool>(cond);						\
		if(_aeEvalBool)													\
		{																\
			AELOG_WARN(pLogger, message);								\
		}																\
	}
#else
	#define AELOG_WARN(pLogger, message)
	#define AELOG_CONDITIONAL_WARN(pLogger, message, cond)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_ERROR))
	//Log error-level message
	#define AELOG_ERROR(pLogger, message) AELOG_MESSAGE(pLogger, Accevo::Logger::AELOGLEVEL_ERROR, message)
	#define AELOG_CONDITIONAL_ERROR(pLogger, message, cond)				\
	{																	\
		bool _aeEvalBool = static_cast<bool>(cond);						\
		if(_aeEvalBool)													\
		{																\
			AELOG_ERROR(pLogger, message);								\
		}																\
	}
#else
	#define AELOG_ERROR(pLogger, message)
	#define AELOG_CONDITIONAL_ERROR(pLogger, message, cond)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_ERROR))
	//Log fatal-level message
	#define AELOG_FATAL(pLogger, message) AELOG_MESSAGE(pLogger, Accevo::Logger::AELOGLEVEL_FATAL, message)
	#define AELOG_CONDITIONAL_FATAL(pLogger, message, cond)				\
	{																	\
		bool _aeEvalBool = static_cast<bool>(cond);						\
		if(_aeEvalBool)													\
		{																\
			AELOG_FATAL(pLogger, message);								\
		}																\
	}
#else
	#define AELOG_FATAL(pLogger, message)
	#define AELOG_CONDITIONAL_FATAL(pLogger, message, cond)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL != AE_LOG_OUTPUT_OFF))
	#define AELOG_MESSAGE(pLogger, level, message) ((pLogger)->LogMessage(level, __LINE__, __FILE__, __FUNCTION__, message))
	#define AELOG_CONDITIONAL(pLogger, level, message, cond)			\
	{																	\
		bool _aeEvalBool = static_cast<bool>(cond);						\
		if(_aeEvalBool)													\
		{																\
			AELOG_MESSAGE(pLogger, level, message);						\
		}																\
	}
#else
	#define AELOG_MESSAGE(pLogger, level, message)
	#define AELOG_CONDITIONAL(pLogger, level, message, cond)
#endif


/*****************************************************************	
	Conditional logging statements that also execute arbitrary code
	in the event that the condition checked for is true.  Compile time
	logging	level can still remove logging statement as needed, but
	the arbitrary code will always be present.
******************************************************************/
#define AELOG_CONDITIONAL_CODE_TRACE(pLogger, message, cond, code)	\
{																	\
	bool _aeEvalBool = static_cast<bool>(cond);						\
	if(_aeEvalBool)													\
	{																\
		AELOG_TRACE(pLogger, message);								\
		code;														\
	}																\
}																	

#define AELOG_CONDITIONAL_CODE_DEBUG(pLogger, message, cond, code)	\
{																	\
	bool _aeEvalBool = static_cast<bool>(cond);						\
	if(_aeEvalBool)													\
	{																\
		AELOG_DEBUG(pLogger, message);								\
		code;														\
	}																\
}																	

#define AELOG_CONDITIONAL_CODE_INFO(pLogger, message, cond, code)	\
{																	\
	bool _aeEvalBool = static_cast<bool>(cond);						\
	if(_aeEvalBool)													\
	{																\
		AELOG_INFO(pLogger, message);								\
		code;														\
	}																\
}

#define AELOG_CONDITIONAL_CODE_WARN(pLogger, message, cond, code)	\
{																	\
	bool _aeEvalBool = static_cast<bool>(cond);						\
	if(_aeEvalBool)													\
	{																\
		AELOG_WARN(pLogger, message);								\
		code;														\
	}																\
}

#define AELOG_CONDITIONAL_CODE_ERROR(pLogger, message, cond, code)	\
{																	\
	bool _aeEvalBool = static_cast<bool>(cond);						\
	if(_aeEvalBool)													\
	{																\
		AELOG_ERROR(pLogger, message);								\
		code;														\
	}																\
}

#define AELOG_CONDITIONAL_CODE_FATAL(pLogger, message, cond, code)	\
{																	\
	bool _aeEvalBool = static_cast<bool>(cond);						\
	if(_aeEvalBool)													\
	{																\
		AELOG_FATAL(pLogger, message);								\
		code;														\
	}																\
}


#endif