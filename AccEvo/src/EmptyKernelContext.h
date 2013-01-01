//David Oguns
//February 14, 2011

#ifndef _EMPTYKERNELCONTEXT_H_
#define _EMPTYKERNELCONTEXT_H_

#include "KernelContext.h"

namespace Accevo
{
	class EmptyKernelContext : public KernelContext
	{
	public:
		//called when context is about to be run
		virtual void Start() {  };
		//called when context is about to not be run
		virtual void Stop() {  };

		//called before kernel processes are
		virtual void PreUpdate(float dt) { };
		virtual void Update(float dt) { };
		virtual void PostUpdate(float dt) { };

	protected:
	};
};

#endif