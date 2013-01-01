//David Oguns
//StringIntern.cpp
//February 27, 2011

#include "StringIntern.h"


namespace Accevo
{
	Map<const char *, boost::shared_ptr<std::string>>		g_InternedStrings;

	//returns the single copy of a string to be held in memory
	char const * InternString(char const *string)
	{
		boost::optional<boost::shared_ptr<std::string>> interned = g_InternedStrings.GetOptional(string);
		if(interned)
		{
			return (*interned)->c_str();
		}
		else
		{
			boost::shared_ptr<std::string> newstring(new std::string(string));
			g_InternedStrings.Insert(newstring->c_str(), newstring);
			return newstring->c_str();
		}
	}

}
