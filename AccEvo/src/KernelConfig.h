//David Oguns
//December 20th, 2017

#pragma once

#include <dxgicommon.h>
#include "KernelContext.h"

/************************************************************************
 * KernelConfiguration object used to initialize the main engine, and passed
 * down to subsystems to consume relevant portions during initialization.
 *
 */

enum DXGI_FORMAT;
namespace Accevo
{
	AUINT32 const AE_SUBSYSTEM_GRAPHICS = 1 << 0;
	AUINT32 const AE_SUBSYSTEM_INPUT = 1 << 1;
	AUINT32 const AE_SUBSYSTEM_SOUND = 1 << 2;

	struct EngineKernelConfiguration
	{
		//parameters pass in from WinMain
		HINSTANCE				hAppInst;
		HINSTANCE				hPrevInst;
		LPSTR					lpCmdLine;
		AINT32					nCmdShow;
		//if passed in, use the given window, otherwise, kernel initializes a new one
		HWND					hWnd;
		//if the underlying program has a clock that it wants updated
		//everytime the kernel updates it's own clock, specify it here
		Clock					*programClock;
		//logger to use for kernel messages
		Logger					*pLogger;
		//main window parameters
		AUINT32					windowWidth;
		AUINT32					windowHeight;
		AUINT32					renderingWidth;
		AUINT32					renderingHeight;
		wchar_t const *			windowTitle;
		ABOOL					bFullscreen;
		ABOOL					bVsync;
		//initial kernel context
		KernelContext			*pInitialContext;
	};

	struct InputConfiguration
	{
		DWORD			keyboardCoopFlags;
		DWORD			mouseCoopFlags;
		AUINT32			nMaxJoysticks;
	};

	struct GraphicsConfigurationRequest
	{
		DXGI_FORMAT		dxgiFormat;
		AUINT32			backBufferWidth;
		AUINT32			backBufferHeight;
		AUINT32			adapterIndex;
		AUINT32			monitorIndex;
	};

	//After the Kernel has started up (opened main window), it may have additional
	//data to pass to sub-systems to start up correctly. This is generically passed
	//in to any/all sub-systems
	struct SubsystemConfiguration
	{
		EngineKernelConfiguration const		*pKernelConfig = nullptr;
		GraphicsConfigurationRequest const	*pGraphicsConfigRequest = nullptr;
		InputConfiguration const			*pInputConfig = nullptr;
	};
}