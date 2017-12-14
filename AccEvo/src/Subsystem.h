//David Oguns
//January 23, 2011
//Subsystem.h


#pragma once

#include "Logger.h"
#include "KernelConfig.h"

namespace Accevo
{
	/*********************************************************************
	This is the interface for all subsystems used by the kernel. Specific 
	subsystems that inherit from this abstract class will(should) probably 
	have a separate Configure()	function that must be called before
	Initialize() to set the initialization parameters.  Configuration may 
	be allowed through the specific subsystem's	constructor as well.

	
	It is not defined here because 
	that function signature will look unpredictably different for every type.
	This function should not do anything for a subsystem where IsReady() 
	returns true.  After Configure is called for a subsystem, 

	*********************************************************************/

	//Though the interface might be not entirely useful, the other use
	//for it is to to prevent EngineKernel.h from including (knowing)
	//concretely of all of the subsystems used by the Kernel which would
	//make it the "uber header" that includes everything under the sun



	class Subsystem
	{
	public:
		Subsystem() {};
		Subsystem(const Subsystem &) = delete;
		Subsystem& operator=(const Subsystem &) = delete;

		//virtual destructor ensures that subclasses get destructed 
		//properly even if being called through this interface.
		virtual ~Subsystem();

		//returns true if and only if subsystem is initialized and ready to be used
		virtual bool IsInitialized() const = 0;
		//initializes the subsystem with the configuration it is currently set to
		//returns true if and only if the subsystem was initialized by this call
		//It should return false if the subsystem is already initialized or if
		//the subsystem failed to initialized fully
		virtual bool Initialize(SubsystemConfiguration const &) = 0;
		//Shuts down the subsystem (and de-initializes it).  Must be 
		//done to reconfigure and reinitialize the subsystem.
		virtual void Shutdown() = 0;

	protected:
	};
}