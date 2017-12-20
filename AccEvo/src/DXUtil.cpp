//David Oguns
//December 14, 2017

#include "DXUtil.h"
#include <memory>
#include <WinBase.h>
#include <boost/format.hpp>

using namespace std;

//Temporary re-definitions of functions from old DirectX SDK libraries
//that I'm trying to not use anymore

wchar_t const * DXGetErrorString(HRESULT const hr)
{
	static thread_local unique_ptr<wchar_t[]> strBuffer{new wchar_t[1024]};
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), strBuffer.get(), 1024, nullptr);
	return strBuffer.get();
}

wchar_t const * DXGetErrorDescription(HRESULT const hr)
{
	static thread_local unique_ptr<wchar_t[]> strBuffer{new wchar_t[1024]};
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), strBuffer.get(), 1024, nullptr);
	return strBuffer.get();
}