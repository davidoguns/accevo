//David Oguns
//February 8, 2011

#ifndef _PROCESSMANAGER_H_
#define _PROCESSMANAGER_H_

#include "Process.h"
#include "HandleManager.h"

#include <list>

namespace Accevo
{
	class ProcessManager
	{
	protected:
		/********************************************************
			PIDs are not used externally since they are O(logN)
			access to the handle, which in turn is O(1) to get
			to the relevant PCB.  Instead, when a process is created
			a handle to the PCB is returned and used when clients
			need to control the happenings of a process.
		********************************************************/
		//typedef std::list<PID>					PIDLIST;

		struct ProcessControlBlock
		{
			//pointer to process itself
			Process			*pProcess;
			//used to mark processes that shouldn't be updated !running = paused/stopped
			bool			running;
			//used to mark processes that should be killed this frame
			//this prevents duplication of killed process in data
			//structures used to kill processes at the right point
			//during execution of the kernel
			bool			killed;
			//marks a process as having been started.  Controls whether or not Stop
			//is called when the process is killed or being cleaned up.
			bool			started;
		};
		typedef HandleManager<ProcessControlBlock>					PCBHANDLEMGR;
		typedef std::list<Handle>									PCBHANDLELIST;

	public:
		//default constructor creates a process manager with no
		//processes in it.  Make constructor explicit as to avoid
		//implicit conversion use
		explicit ProcessManager(Logger *pLogger);
		//destructor stops all processes that have been started
		//and destroys memory for all process control blocks.
		~ProcessManager();

		//Adds a process by creating a unique process control block for it.
		//The process itself is NOT copied so the same process can be added
		//to the kernel twice for ill effect (or potentially desired)
		Handle AddProcess(Process *pProcess, bool running);
		
		//add process to kill list.  killed processes
		//are removed at the end of a kernel loop iteration
		void KillProcess(Handle const & hProcess);

		//sets whether or not a process is running or not during
		//kernel iterations.  New flag doesn't apply until next
		//iteration.
		void SetRunning(Handle const & hProcess, bool running);

		//called at the start of a frame so anything the manager
		//needs to do can be done.  An exaple might be adding
		//processes that need to be added, and starting them up.
		void StartFrame();
		//called at the end of a frame so the manager can do anything
		//that needs to be done like removing processes that need
		//to be killed and calling stop on them.
		void EndFrame();

		//calls PreUpdate() on all processes set to be run this frame
		void PreUpdateAll(float dt);
		//calls PostUpdate() on all processes set to be run this frame
		void PostUpdateAll(float dt);

	protected:
		//handles to all processes - execution order is not preserved in this container
		PCBHANDLEMGR								m_allProcessBlocks;
		//list of processes that are being run for a single frame.  It is
		//constructed at the start of a frame's execution and does not change
		//midway through (even as processes are added or deleted
		PCBHANDLELIST								m_runningProcessList;
		//list of process that should be added for the next frame
		//this is needed over the storage of the handle manager to
		//preserve the order of execution for processes
		PCBHANDLELIST								m_allProcesses;
		//processes to remember to add during StartFrame()
		PCBHANDLELIST								m_addedProcesses;
		//Logger for general shennanigans in process managers
		Logger										*m_pLogger;
	};
}

#endif