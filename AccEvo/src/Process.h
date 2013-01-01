//David Oguns
//February 6, 2011

#ifndef _AEPROCESS_H_
#define _AEPROCESS_H_


namespace Accevo
{
	/********************************************************
		A process is meant to abstractly wrap work that needs 
		to be done in support of a running kernel context. In
		theory, all work can be pushed off to processes, but 
		the idea is that the context's update method contains
		the core responsibilities of the game at any point.

		Processes can live beyond the lifetime of the context
		that spawned it, but 
	********************************************************/
	typedef unsigned int				PID;

	class Process
	{
	public:
		//allow clean up of derived classes
		virtual ~Process();

		//called before the main context updates
		virtual void PreUpdate(float dt);
		//called after the main context updates
		virtual void PostUpdate(float dt);

		//called when process is first made active
		virtual void Start(PID const & pid);
		//called to stop the process from running
		virtual void Stop();

		//returns true if process is done running.  this is how
		//processes should kill themselves.  Stop will still
		//be called on the process at the end of the frame.
		virtual bool IsFinished();

	private:
		
	};
}


#endif