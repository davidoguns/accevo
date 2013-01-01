//David Oguns
//December 26, 2010

#ifndef _DXUTIL_H_
#define _DXUTIL_H_

#include "GlobalInclude.h"
#include <D3DX11.h>
#include <DxErr.h>
#include <sstream>
#include "Logger.h"


/***************************************************************
	USAGE WARNING:

	Users of this macros should realize that C++ macro replacements
	are strictly textual which may cause arguments to be evaluated
	more than once.  With these macros, the hr argument is used
	either two or three times depending on if the conditional form
	is used.  If the caller inserts a DirectX functional call into
	the "macro call" like so:
		AELOG_DXERR_ERROR(m_pLogger, L"DirectX call failed!!!", 
			pDevice->DoStuff());
	Then pDevice->DoStuff() will actually get called each time hr
	is used in the expansion.  Though the call will likely have the
	same return value each time, this is not performant, and certainly
	not intended.  It is recommended that the caller use a local hr
	variable to store the return value of the call, and pass that 
	into the macro instead.  Like so:
		HRESULT hr = pDevice->DoStuff();
		AELOG_DXERR_ERROR(m_pLogger, L"DirectX call failed!!!",	hr);


***************************************************************/

#if(defined(_DEBUG))
	#define DX_SET_DEBUG_NAME(pObj, name)											\
	{																				\
		pObj->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);		\
	}
#else
	#define DX_SET_DEBUG_NAME(pObj, name)
#endif



/*****************************************************************
	The following macros are used to log the description of return
	values from DirectX function calls.  It is assumed the HRESULT
	is an error.
*****************************************************************/

/************************************************************
	The next set of macros are useful for condensing the return
	values of DirectX functions (HRESULTS) and logging the
	reason for failure conditionally.  These can be compiled out
	as well, but probably will exist in most builds for critical
	function calls that fail.
*************************************************************/


#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_TRACE))
	#define AELOG_DXERR_TRACE(pLogger, wstr_msg, hr)			\
	{															\
		std::wostringstream out;								\
		out << wstr_msg	<< std::endl							\
			<< L"\t DX Error String: "<< DXGetErrorString(hr)	\
			<<  std::endl										\
			<< L"\t DX Error Description: "						\
			<< DXGetErrorDescription(hr);					\
		AELOG_TRACE(pLogger, out.str().c_str());						\
	}
	#define AELOG_DXERR_CONDITIONAL_TRACE(pLogger, wstr_msg, hr)	\
	{																\
		if(FAILED(hr))												\
		{															\
			AELOG_DXERR_TRACE(pLogger, wstr_msg, hr);				\
		}															\
	}
#else
	#define AELOG_DXERR_TRACE(pLogger, wstr_msg, hr)
	#define AELOG_DXERR_CONDITIONAL_TRACE(pLogger, wstr_msg, hr)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_DEBUG))
	#define AELOG_DXERR_DEBUG(pLogger, wstr_msg, hr)			\
	{															\
		std::wostringstream out;								\
		out << wstr_msg	<< std::endl							\
			<< L"\t Error String: "<< DXGetErrorString(hr)		\
			<< std::endl										\
			<< L"\t Error Description: "						\
			<< DXGetErrorDescription(hr);						\
		AELOG_DEBUG(pLogger, out.str().c_str());				\
	}
	#define AELOG_DXERR_CONDITIONAL_DEBUG(pLogger, wstr_msg, hr)	\
	{																\
		if(FAILED(hr))												\
		{															\
			AELOG_DXERR_DEBUG(pLogger, wstr_msg, hr);				\
		}															\
	}
#else
	#define AELOG_DXERR_DEBUG(pLogger, wstr_msg, hr)
	#define AELOG_DXERR_CONDITIONAL_DEBUG(pLogger, wstr_msg, hr)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_INFO))
	#define AELOG_DXERR_INFO(pLogger, wstr_msg, hr)			\
	{															\
		std::wostringstream out;								\
		out << wstr_msg	<< std::endl							\
			<< L"\t Error String: "<< DXGetErrorString(hr)		\
			<< std::endl										\
			<< L"\t Error Description: "						\
			<< DXGetErrorDescription(hr);						\
		AELOG_INFO(pLogger, out.str().c_str());					\
	}
	#define AELOG_DXERR_CONDITIONAL_INFO(pLogger, wstr_msg, hr)	\
	{															\
		if(FAILED(hr))											\
		{														\
			AELOG_DXERR_INFO(pLogger, wstr_msg, hr);			\
		}														\
	}
#else
	#define AELOG_DXERR_INFO(pLogger, wstr_msg, hr)
	#define AELOG_DXERR_CONDITIONAL_INFO(pLogger, wstr_msg, hr)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_WARN))
	#define AELOG_DXERR_WARN(pLogger, wstr_msg, hr)			\
	{															\
		std::wostringstream out;								\
		out << wstr_msg	<< std::endl							\
			<< L"\t Error String: "<< DXGetErrorString(hr)		\
			<< std::endl										\
			<< L"\t Error Description: "						\
			<< DXGetErrorDescription(hr);						\
		AELOG_WARN(pLogger, out.str().c_str());					\
	}
	#define AELOG_DXERR_CONDITIONAL_WARN(pLogger, wstr_msg, hr)	\
	{															\
		if(FAILED(hr))											\
		{														\
			AELOG_DXERR_WARN(pLogger, wstr_msg, hr);			\
		}														\
	}
#else
	#define AELOG_DXERR_WARN(pLogger, wstr_msg, hr)
	#define AELOG_DXERR_CONDITIONAL_WARN(pLogger, wstr_msg, hr)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_ERROR))
	#define AELOG_DXERR_ERROR(pLogger, wstr_msg, hr)			\
	{															\
		std::wostringstream out;								\
		out << wstr_msg	<< std::endl							\
			<< L"\t Error String: "<< DXGetErrorString(hr)		\
			<< std::endl										\
			<< L"\t Error Description: "						\
			<< DXGetErrorDescription(hr);						\
		AELOG_ERROR(pLogger, out.str().c_str());				\
	}
	#define AELOG_DXERR_CONDITIONAL_ERROR(pLogger, wstr_msg, hr)\
	{															\
		if(FAILED(hr))											\
		{														\
			AELOG_DXERR_ERROR(pLogger, wstr_msg, hr);			\
		}														\
	}
#else
	#define AELOG_DXERR_ERROR(pLogger, wstr_msg, hr)
	#define AELOG_DXERR_CONDITIONAL_ERROR(pLogger, wstr_msg, hr)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL >= AE_LOG_OUTPUT_FATAL))
	#define AELOG_DXERR_FATAL(pLogger, wstr_msg, hr)			\
	{															\
		std::wostringstream out;								\
		out << wstr_msg	<< std::endl							\
			<< L"\t Error String: "<< DXGetErrorString(hr)		\
			<< std::endl										\
			<< L"\t Error Description: "						\
			<< DXGetErrorDescription(hr);						\
		AELOG_FATAL(pLogger, out.str().c_str());				\
	}
	#define AELOG_DXERR_CONDITIONAL_FATAL(pLogger, wstr_msg, hr)\
	{															\
		if(FAILED(hr))											\
		{														\
			AELOG_DXERR_FATAL(pLogger, wstr_msg, hr);			\
		}														\
	}
#else
	#define AELOG_DXERR_FATAL(pLogger, wstr_msg, hr)
	#define AELOG_DXERR_CONDITIONAL_FATAL(pLogger, wstr_msg, hr)
#endif

#if(!defined(AE_LOG_OUTPUT_LEVEL) || (AE_LOG_OUTPUT_LEVEL != AE_LOG_OUTPUT_OFF))
	#define AELOG_DXERR(pLogger, level, wstr_msg, hr)			\
	{															\
		std::wostringstream out;								\
		out << wstr_msg	<< std::endl							\
			<< L"\t Error String: "<< DXGetErrorString(hr)		\
			<< std::endl										\
			<< L"\t Error Description: "						\
			<< DXGetErrorDescription(hr);						\
		AELOG_MESSAGE(pLogger, level, out.str().c_str());		\
	}	
	#define AELOG_DXERR_CONDITIONAL(pLogger, level, wstr_msg, hr)	\
	{																\
		if(FAILED(hr))												\
		{															\
			AELOG_DXERR(pLogger, level, wstr_msg, hr);				\
		}															\
	}
#else
	#define AELOG_DXERR(pLogger, level, wstr_msg, hr)
	#define AELOG_DXERR_CONDITIONAL(pLogger, level, wstr_msg, hr)
#endif





/*******************************************************************
	The following macros are useful when there is a need to 
	conditionally check a DirectX HRESULT and log it in the case
	of failure, as well as executing arbitrary code (i.e. return false, 
	clean up memory, etc).  These macros are not conditional for the 
	code that must be executed in the event of a failure.  However,
	the logging falls back to macros that may end up being compiled 
	out assocated with the logging level of the macro used.
*******************************************************************/

#define AELOG_DXERR_CONDITIONAL_CODE_TRACE(pLogger, wstr_msg, hr, code)	\
{																		\
	HRESULT _evalhr = (hr);												\
	if(FAILED(_evalhr))													\
	{																	\
		AELOG_DXERR_TRACE(pLogger, wstr_msg, _evalhr);					\
		code;															\
	}																	\
}

#define AELOG_DXERR_CONDITIONAL_CODE_DEBUG(pLogger, wstr_msg, hr, code)	\
{																		\
	HRESULT _evalhr = (hr);												\
	if(FAILED(_evalhr))													\
	{																	\
		AELOG_DXERR_DEBUG(pLogger, wstr_msg, _evalhr);					\
		code;															\
	}																	\
}

#define AELOG_DXERR_CONDITIONAL_CODE_INFO(pLogger, wstr_msg, hr, code)	\
{																		\
	HRESULT _evalhr = (hr);												\
	if(FAILED(evalhr))													\
	{																	\
		AELOG_DXERR_INFO(pLogger, wstr_msg, _evalhr);					\
		code;															\
	}																	\
}

#define AELOG_DXERR_CONDITIONAL_CODE_WARN(pLogger, wstr_msg, hr, code)	\
{																		\
	HRESULT _evalhr = (hr);												\
	if(FAILED(_evalhr))													\
	{																	\
		AELOG_DXERR_WARN(pLogger, wstr_msg, _evalhr);					\
		code;															\
	}																	\
}

#define AELOG_DXERR_CONDITIONAL_CODE_ERROR(pLogger, wstr_msg, hr, code)	\
{																		\
	HRESULT _evalhr = (hr);												\
	if(FAILED(_evalhr))													\
	{																	\
		AELOG_DXERR_ERROR(pLogger, wstr_msg, _evalhr);					\
		code;															\
	}																	\
}

#define AELOG_DXERR_CONDITIONAL_CODE_FATAL(pLogger, wstr_msg, hr, code)	\
{																		\
	HRESULT _evalhr = (hr);												\
	if(FAILED(_evalhr))													\
	{																	\
		AELOG_DXERR_FATAL(pLogger, wstr_msg, _evalhr);					\
		code;															\
	}																	\
}

#define AELOG_DXERR_CONDITIONAL_CODE(pLogger, level, wstr_msg, hr, code)	\
{																			\
	HRESULT _evalhr = (hr);													\
	if(FAILED(_evalhr))														\
	{																		\
		AELOG_DXERR(pLogger, level, wstr_msg, _evalhr);						\
		code;																\
	}																		\
}

struct lessThanStr
{
	bool operator()(const char *str1, const char *str2) const
	{
		return strcmp(str1, str2) < 0;
	}
};

//standard procedure for releasing COM interface pointers
#define DX_RELEASE(x) { if(x){ x->Release();x = nullptr; } }

#endif

