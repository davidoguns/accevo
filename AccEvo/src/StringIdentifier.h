//David Oguns
//January 23, 2011

#ifndef _HASHED_STRING_H_
#define _HASHED_STRING_H_


#include <boost/crc.hpp>
#include <string>
#include "aetypes.h"

namespace Accevo
{
	typedef AUINT32		StringIdentifier;

	struct StringIdentifierCmp
	{
		bool operator()(StringIdentifier const &rhs, StringIdentifier const &lhs) const
		{
			return rhs < lhs;
		}
	};


#if(defined(_DEBUG) || defined(DEBUG))
	void AddSID(StringIdentifier sid, char const *string);
	void AddSID(StringIdentifier sid, wchar)_t const *string);
#endif

	#define SID(id) (Accevo::HashString(id))

	//runtime hashing of string
	StringIdentifier HashString(char const *string);
	StringIdentifier HashString(wchar_t const *string);
};

#endif