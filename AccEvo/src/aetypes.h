//David Oguns
//January 25, 2011
//aetypes.h

#ifndef _AETYPES_H_
#define _AETYPES_H_

#include <limits>

//#include <cstdint>	//would be a useful non MSVC++ fallback

/**
  * Everything is built up based on POD types. Making these portable
  * is the key start to making every other type built on top of them
  * portable. I'm sure there will be other challenges in making the
  * composite types portable.
  **/
namespace Accevo {
	//only valid in MSVC++ 9.0 +
	typedef __int8					AINT8;
	typedef __int16					AINT16;
	typedef	__int32					AINT32;
	typedef __int64					AINT64;
	typedef unsigned __int8			AUINT8;
	typedef unsigned __int16		AUINT16;
	typedef unsigned __int32		AUINT32;
	typedef unsigned __int64		AUINT64;

	typedef unsigned char			AUCHAR8;
	typedef signed char				ACHAR8;
	typedef wchar_t					AUCHAR16;

	typedef bool					ABOOL;
	const	ABOOL					ATRUE = true;
	const	ABOOL					AFALSE = false;

	typedef float					AFLOAT32;
	typedef double					AFLOAT64;
	typedef AFLOAT64				ADOUBLE;
}

#endif