//David Oguns
//January 17, 2011
//GameKernel.h


#ifndef _ENGINEKERNEL_H_
#define _ENGINEKERNEL_H_

#include "GlobalInclude.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include <boost\noncopyable.hpp>
#include <iostream>

#include "aetypes.h"
#include "Logger.h"
#include "Clock.h"
#include "Subsystem.h"
#include "ProcessManager.h"
#include "KernelContext.h"

namespace Accevo
{
	enum AE_KERNEL_EXIT_STATUS : AINT32 {
		AEKES_ERROR = -1,
		AEKES_SUCCESS = 0,
	};

	struct EngineKernelConfiguration
	{
		//parameters pass in from WinMain
		HINSTANCE				hAppInst;
		HINSTANCE				hPrevInst;
		LPSTR					lpCmdLine;
		AINT32					nCmdShow;
		//if the underlying program has a clock that it wants updated
		//everytime the kernel updates it's own clock, specify it here
		Clock					*programClock;
		//logger to use for kernel messages
		Logger					*pLogger;
		//main window parameters
		AUINT32					windowWidth;
		AUINT32					windowHeight;
		wchar_t const *			windowTitle;
		ABOOL					bFullscreen;
		ABOOL					bVsync;
		//initial kernel context
		KernelContext			*pInitialContext;
	};

	class EngineKernel : boost::noncopyable
	{
	private:
		EngineKernel(const EngineKernelConfiguration &kernelConfig);
		~EngineKernel();

		ABOOL IsReady() const {return m_bIsInitialized; }
		void Shutdown();

		void HandleInput();

		HWND CreateMainWindow();
		void ResizeMainWindow(AUINT32 width, AUINT32 height);
		ABOOL StartSubsystems();
		void StopSubsystems();
		
		ABOOL StartGraphicsSubsystem();
		ABOOL StartInputSubsystem();

	public:
		static EngineKernel* CreateKernel(const EngineKernelConfiguration &kernelConfig);
		static EngineKernel* GetKernel() { return m_pKernel; }
		static void ShutdownKernel();	//TODO: shouldn't be public...immediately invalidates stuff
		
		//static window procedure to handle the windows message pump
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		AINT32 Run();

		void Stop();	//safe way to get kernel to stop

		//return a pointer to the graphics subsystem
		Subsystem * GetGraphicsSubSystem() const { return m_pGraphics; }
		//return a pointer to the input subsystem
		Subsystem * GetInputSubSystem() const { return m_pInput; }
		//gets the logger for the kernel, useable fallback if a log call is passed null logger
		Logger * GetLogger() const { return m_pLogger; }

		//Gets the user process manager
		ProcessManager * GetProcessManager() { return &m_userProcessMgr; }

	protected:
		static EngineKernel				*m_pKernel;		//kernel logger
		ABOOL							m_bIsInitialized;	//is kernel ready to be run or already running
		EngineKernelConfiguration		m_config;		//configuration
		HWND							m_hWnd;			//handle to application window

		Logger							*m_pLogger;		//logger that kernel uses
		Clock							m_clock;		//timer to use for kernel since creation
		AFLOAT32						m_dt;			//delta time since last frame (only valid when running)
		ABOOL							m_bExit;		//exit flag sets to true when we should be closing

		Subsystem						*m_pGraphics;	//graphics subsystem
		Subsystem						*m_pInput;		//input subsystem
		//vector of live subsystems to make cleaning up easier in the event of failure during start up
		std::vector<Subsystem *>		m_vLiveSubSystems;

		//kernel process running		
		KernelContext					*m_pRunningContext;
		KernelContext					*m_pNextRunningContext;
		//processes used by the kernel itself
		ProcessManager					m_kernelProcessMgr;
		//processes used by the user
		ProcessManager					m_userProcessMgr;
	};

}	//namespace Accevo

#define ENGINE_LOGGER (EngineKernel::GetKernel()->GetLogger())

#endif
