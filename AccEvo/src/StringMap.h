//David Oguns
//February 12, 2011

#ifndef _STRINGMAP_H_
#define _STRINGMAP_H_

#include "Map.h"
#include "StringIdentifier.h"

namespace Accevo
{
	template<class T>
	class StringMap : public Map<StringIdentifier, T, StringIdentifierCmp> { };
}

#endif