//David Oguns
//February 13, 2011

#include "GlobalInclude.h"
#include "ProcessManager.h"
#include "NullLogger.h"
#include <iterator>

namespace Accevo
{	
	//default constructor creates a process manager with no
	//processes in it
	ProcessManager::ProcessManager(Logger *pLogger) :
		m_allProcessBlocks(pLogger),
		m_pLogger(pLogger)
	{

	}

	//destructor stops all processes that have been started
	//and destroys memory for all process control blocks.
	ProcessManager::~ProcessManager()
	{

	}

	//Creates the process control block and adds it to list of handles
	//to be started at the start of the next frame
	Handle ProcessManager::AddProcess(Process *pProcess, bool running)
	{
		Handle hProcess = m_allProcessBlocks.CreateEntry();
		if(!hProcess.IsNull())
		{
			//get memory of newly created process block
			ProcessControlBlock &pcb = m_allProcessBlocks.GetData(hProcess);
			//add to list to be added in next start frame (in case this is called mid-frame)
			m_addedProcesses.push_back(hProcess);
		}
		return hProcess;
	}
		
	//removes a process from the all processes list
	void ProcessManager::KillProcess(Handle const & hProcess)
	{
		PCBHANDLELIST::iterator toKillItr = std::find(m_allProcesses.begin(), m_allProcesses.end(), hProcess);
		boost::optional<ProcessControlBlock &> pcbToKill;
		if(toKillItr == m_allProcesses.end())
		{	//not found in all processes, check added processes
			toKillItr = std::find(m_addedProcesses.begin(), m_addedProcesses.end(), hProcess);
			if(toKillItr == m_addedProcesses.end())
			{	//process not found in either list means it doesn't exist
				AELOG_WARN(m_pLogger, L"Cannot remove process that does not exist!!");
			}
			else
			{	//remove if found in added processes
				m_addedProcesses.erase(toKillItr);
			}
		}
		else
		{
			m_allProcesses.erase(toKillItr);
		}
	}

	//sets whether or not a process is running or not during
	//kernel iterations.  New flag doesn't apply until next
	//iteration.
	void ProcessManager::SetRunning(Handle const & hProcess, bool running)
	{
		//change flag for running or not
		boost::optional<ProcessControlBlock &> pcbRet = m_allProcessBlocks.GetDataOptional(hProcess);
		if(pcbRet)
		{
			pcbRet->running = true;
		}
		else
		{
			AELOG_WARN(m_pLogger, "No process found to change active state for given handle!!!");
		}
	}

	//called at the start of a frame so anything the manager
	//needs to do can be done.  An exaple might be adding
	//processes that need to be added, and starting them up.
	void ProcessManager::StartFrame()
	{	
		m_runningProcessList.clear();
		//select processes to run this frame
		std::copy(m_allProcesses.begin(), m_allProcesses.end(), 
			std::back_insert_iterator<PCBHANDLELIST>(m_runningProcessList));

		//start up processes in added list and move to running process list
		std::for_each(m_addedProcesses.begin(), m_addedProcesses.end(),
			//capture running process list since we are adding to it
			[&](Handle const & pcbHandle)
			{
				ProcessControlBlock &pcb = m_allProcessBlocks.GetData(pcbHandle);
				//start the process -- use the handle index as the PID
				pcb.pProcess->Start(pcbHandle.GetIndex());
				pcb.started = true;
				pcb.killed = false;
				//add to running process list
				m_runningProcessList.push_back(pcbHandle);
			}
		);
		//clear to add list
		m_addedProcesses.clear();
	}

	//called at the end of a frame so the manager can do anything
	//that needs to be done like removing processes that need
	//to be killed and calling stop on them.
	void ProcessManager::EndFrame()
	{
		//iterate over all processes that ran this frame (even if it's not attached)
		std::for_each(m_allProcesses.begin(), m_allProcesses.end(),
			[&] (Handle const & hProcess)
			{
				ProcessControlBlock &pcb = m_allProcessBlocks.GetData(hProcess);
				if(pcb.pProcess->IsFinished() || pcb.killed)
				{
					pcb.pProcess->Stop();
					pcb.killed = true;

					//remove block from handle manager
					m_allProcessBlocks.RemoveEntry(hProcess);
					//remove handle from all processes
					m_allProcesses.remove(hProcess);
				}
			}
		);

		/*
		for(PCBHANDLELIST::iterator pcbHandleItr = m_allProcesses.begin();
			pcbHandleItr != m_allProcesses.end();
			++pcbItr)
		{
			ProcessControlBlock *pcb = *pcbItr;
			//if process is done running and or has been killed manually
			if(pcb->pProcess->IsFinished() || pcb->killed)
			{
				//call stop on process
				pcb->pProcess->Stop();
				//finally remove from list of all processes
				m_allProcesses.erase(pcbItr);
				//free memory used by PCB
				delete *pcbItr;
				//WARNING:  at this point, if the process was run during this frame
				//			m_runningProcessList() will still hold a dangling pointer
				//			to the PCB.  It can be cleared here, but is cleared a bit
				//			later so processes that did run can be examined until replaced.
			}
		}
		*/
	}

	//calls PreUpdate() on all processes set to be run this frame
	void ProcessManager::PreUpdateAll(float dt)
	{
		//call pre-update on all processes
		std::for_each(m_runningProcessList.begin(), m_runningProcessList.end(),
			[&] (Handle const & hProcess)
			{
				ProcessControlBlock &pcb = m_allProcessBlocks.GetData(hProcess);
				pcb.pProcess->PreUpdate(dt);
			}
		);
	}

	//calls PostUpdate() on all processes set to be run this frame
	void ProcessManager::PostUpdateAll(float dt)
	{
		//call post-update on all processes
		std::for_each(m_runningProcessList.begin(),	m_runningProcessList.end(),
			[&] (Handle const & hProcess)
			{
				ProcessControlBlock &pcb = m_allProcessBlocks.GetData(hProcess);
				pcb.pProcess->PostUpdate(dt);
			}
		);
	}
}
