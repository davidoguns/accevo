//David Oguns
//February 6, 2011

#ifndef _AEPROCESS_H_
#define _AEPROCESS_H_


namespace Accevo
{
	/********************************************************
		A process is meant to be a context for which work that needs 
		to be done in support of a running kernel context, or the kernel
		itself is done. In theory, all work can be pushed off to 
		processes, but the idea is that the context's update method 
		contains the core responsibilities of the game at any point.

		Processes can live beyond the lifetime of the context
		that spawned it, but it makes it harder to track who is 
		responsible for starting and stopping them.  Hopefully a long
		running 'game' does not end up process-leaking.
	********************************************************/
	typedef unsigned int				PID;

	class Process
	{
	public:

		//called before the main context updates
		virtual void PreUpdate(float dt) = 0;
		//called after the main context updates
		virtual void PostUpdate(float dt) = 0;

		//called when process is first made active
		virtual void Start(PID const & pid) = 0;
		//called to stop the process from running
		virtual void Stop() = 0;

		//returns true if process is done running.  this is how
		//processes should kill themselves.  Stop will still
		//be called on the process at the end of the frame.
		virtual bool IsFinished() const = 0;

	private:
		
	};
}


#endif