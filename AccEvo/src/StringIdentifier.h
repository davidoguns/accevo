//David Oguns
//January 23, 2011

#ifndef _HASHED_STRING_H_
#define _HASHED_STRING_H_

#include <boost/crc.hpp>
#include <string>

namespace Accevo
{
	typedef unsigned int		StringIdentifier;

	struct StringIdentifierCmp
	{
		bool operator()(StringIdentifier const &rhs, StringIdentifier const &lhs) const
		{
			return rhs < lhs;
		}
	};


#if(defined(_DEBUG) || defined(DEBUG))
	void AddSID(StringIdentifier sid, char const *string);
#endif

	#define SID(id) (Accevo::HashString(id))

	//runtime hashing of string
	StringIdentifier HashString(char const *string);

};

#endif