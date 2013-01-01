//David Oguns
//February 27, 2011

#ifndef _STRINGINTERN_H_
#define _STRINGINTERN_H_

#include "Map.h"
#include <boost/shared_ptr.hpp>

namespace Accevo
{

	//This struct implements a less than function which stl
	//uses to sort object instances in the container classes.
	//Here, I am defining comparison between two c strings.
	struct lessThanStr
	{
		bool operator()(const char *str1, const char *str2) const
		{
			return strcmp(str1, str2) < 0;
		}
	};

	//returns the single copy of a string to be held in memory
	char const * InternString(char const *string);

	//
	extern Map<const char *, boost::shared_ptr<std::string>>		g_InternedStrings;
}

#endif