//David Oguns
//February 6, 2011

#ifndef _KERNELCONTEXT_H_
#define _KERNELCONTEXT_H_

#include "aetypes.h"
#include "GlobalInclude.h"

namespace Accevo
{

	class KernelContext
	{
	public:
		//called when context is about to be run
		virtual void Start() = 0;
		//called when context is about to not be run
		virtual void Stop() = 0;

		//called before kernel processes are 
		virtual void PreUpdate(AFLOAT32 dt) = 0;
		virtual void Update(AFLOAT32 dt) = 0;
		virtual void PostUpdate(AFLOAT32 dt) = 0;

	private:

	};
}

#endif