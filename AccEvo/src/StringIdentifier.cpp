//David Oguns
//February 7, 2011

#include "StringIdentifier.h"
#include "StringMap.h"
#include <string>
#include <cassert>

#include "StringIntern.h"

namespace Accevo
{

#if(defined(_DEBUG) || defined(DEBUG))
	StringMap<const char *>				g_StringIdentifiers;

	void AddSID(StringIdentifier sid, const char *string)
	{
		boost::optional<const char *> prevEntry = g_StringIdentifiers.GetOptional(sid);
		if(prevEntry)
		{
			char const *prevString = *prevEntry;
			//make this assertion in debug builds to catch the small chance
			//of two different strings resulting in the same SID
			assert(strcmp(prevString, string) == 0);
			//no neeed to insert since it's already there
		}
		else
		{
			g_StringIdentifiers.Insert(sid, string);
		}
	}

	//runtime hashing of string
	StringIdentifier HashString(char const *string)
	{
		StringIdentifier sid;
		boost::crc_32_type result;
		result.process_bytes(string, strlen(string));
		sid = result.checksum();
#if(defined(_DEBUG) || defined(DEBUG))
		AddSID(sid, InternString(string));
#endif
		return sid;
	}

#endif
}