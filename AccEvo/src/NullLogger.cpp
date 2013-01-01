//David Oguns
//December 30, 2012

//why a source file for this?  so null logger is a singleton and everyone doesn't have a copy of something new
#include "NullLogger.h"

namespace Accevo
{
	NullLogger		NullLogger::theNullLogger;	//static initialization of the null logger

	//Retrieve the one and only null logger
	Logger * NullLogger::GetInstance()
	{
		return &theNullLogger;
	}


}