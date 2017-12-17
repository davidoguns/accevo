//David Oguns
//January 17 2011
//WinMain.cpp

#define WIN32_LEAN_AND_MEAN
#define AE_LOG_OUTPUT_LEVEL AE_LOG_OUTPUT_TRACE

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>

#include <accevo\EngineKernel.h>

#include <accevo\XmlStreamLogger.h>
#include "WinMain.h"
#include <accevo\Handle.h>
#include <accevo\EmptyKernelContext.h>
#include <boost\format.hpp>

#include "ModelViewContext.h"

using std::wcout;
using std::endl;
using std::wofstream;
using std::wcerr;


int CALLBACK WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	//get current flags for debug reporting
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	//turn flag on for debug reporting memory leaks before app exist
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
	//_crtBreakAlloc = 3303;

	Accevo::Clock::InitClock();
	Accevo::Clock appClock;
	Accevo::XmlStreamLogger logger("log.xml", "kernel", appClock, Accevo::Logger::AELOGLEVEL_TRACE);	
	Accevo::EngineKernelConfiguration kc;
	kc.hAppInst = hThisInst;
	kc.hPrevInst = hPrevInst;
	kc.lpCmdLine = lpCmdLine;
	kc.nCmdShow = nCmdShow;

	kc.pLogger = &logger;
	kc.windowWidth = 1920;
	kc.windowHeight = 1080;
	kc.windowTitle = L"Model Viewer";
	kc.bFullscreen = false;
	kc.bVsync = false;
	kc.programClock = &appClock;
	kc.pInitialContext = new ModelViewContext(&logger);
	
	//create kernel
	Accevo::EngineKernel *kernel = Accevo::EngineKernel::CreateKernel(kc);
	int ret = 0;
	if(kernel)
	{
		ret = kernel->Run();
		Accevo::EngineKernel::ShutdownKernel();
	}

	//free memory
	delete kc.pInitialContext;
	return ret;
}