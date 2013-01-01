//David Oguns
//March 12, 2012
//CoreInput.h

/*******************************************************
	This class is intended to replace DirectInput.h
*******************************************************/

#pragma once

#include <boost/noncopyable.hpp>
#include "Subsystem.h"
#include "Logger.h"

namespace Accevo
{
	class CoreInput : public boost::noncopyable
	{
	public:
		//constructor to initialize underlying input API, which by no surprise is DirectInput8
		CoreInput(Logger *pLogger);
		//subsystems have virtual destructors
		virtual ~CoreInput();



	protected:
	};
}