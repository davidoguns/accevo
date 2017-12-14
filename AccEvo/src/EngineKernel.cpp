//David Oguns
//EngineKernel.cpp

#include "EngineKernel.h"
#include "GraphicsLayer.h"
#include "DirectInput.h"

#include <boost/format.hpp>

#define AE_LOG_OUTPUT_LEVEL AE_LOG_OUTPUT_TRACE

namespace Accevo
{
	wchar_t const * lpszWindowClassName = L"Accevo Engine";

	//initialize static instance of singleton class
	EngineKernel *EngineKernel::m_pKernel = nullptr;

	//static creation of kernel can only be called once
	EngineKernel* EngineKernel::CreateKernel(const EngineKernelConfiguration &kernelConfig)
	{
		if(m_pKernel)
		{
			AELOG_ERROR(m_pKernel->m_pLogger, "Create kernel called twice!!!  Exiting...");
			ShutdownKernel();
			delete m_pKernel;
			m_pKernel = nullptr;
		}
		else
		{
			m_pKernel = new EngineKernel(kernelConfig);
			if(!m_pKernel->IsReady())
			{
				AELOG_FATAL(kernelConfig.pLogger, L"Could not create Accevo kernel!");
				m_pKernel->Shutdown();
				delete m_pKernel;
				m_pKernel = nullptr;
			}
		}
		return m_pKernel;
	}

	void EngineKernel::ShutdownKernel()
	{
		m_pKernel->Shutdown();
		delete m_pKernel;
		m_pKernel = nullptr;
	}

	EngineKernel::EngineKernel(const EngineKernelConfiguration &kernelConfig) :
		m_config(kernelConfig),
		m_pLogger(m_config.pLogger),		//duplicate for easy access
		m_pGraphics(nullptr),
		m_pInput(nullptr),
		m_bIsInitialized(AFALSE),
		m_bExit(AFALSE),
		m_kernelProcessMgr(m_pLogger),
		m_userProcessMgr(m_pLogger),
		m_pRunningContext(nullptr),
		m_pNextRunningContext(nullptr)
	{
		AELOG_DEBUG(m_pLogger, "Constructing engine kernel...");

		if(m_config.programClock)
		{
			m_clock.AddChild(m_config.programClock);
		}

		if (!m_config.hWnd)
		{
			m_config.hWnd = CreateMainWindow();
			if (!m_config.hWnd)
			{
				AELOG_ERROR(m_pLogger, L"Could not create main window");
				//no need to let this function go further
				return;
			}
		}

		if(!StartSubsystems())
		{
			AELOG_ERROR(m_pLogger, L"Could not start engine subsystems");
			//no need to let this function go further
			return;
		}

		m_bIsInitialized = ATRUE;
	}

	EngineKernel::~EngineKernel()
	{

	}

	ABOOL EngineKernel::StartSubsystems()
	{
		StopSubsystems();

		GraphicsConfigurationRequest graphicsConfig;
		graphicsConfig.adapterIndex = 0;
		graphicsConfig.monitorIndex = 0;
		graphicsConfig.dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		graphicsConfig.backBufferWidth = m_config.renderingWidth;
		graphicsConfig.backBufferHeight = m_config.renderingHeight;

		InputConfiguration inputConfig;
		inputConfig.keyboardCoopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
		inputConfig.mouseCoopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
		inputConfig.nMaxJoysticks = 16;

		SubsystemConfiguration subSystemConfig;
		subSystemConfig.pKernelConfig = &m_config;
		subSystemConfig.pGraphicsConfigRequest = &graphicsConfig;
		subSystemConfig.pInputConfig = &inputConfig;

		if(!StartGraphicsSubsystem(subSystemConfig))
		{
			AELOG_FATAL(m_pLogger, "Could not start graphics subsystem");
			return AFALSE;
		}
		if(!StartInputSubsystem(subSystemConfig))
		{
			AELOG_FATAL(m_pLogger, "Could not start input subsystem");
			return AFALSE;
		}

		return ATRUE;
	}

	ABOOL EngineKernel::StartGraphicsSubsystem(SubsystemConfiguration const & config)
	{
		AELOG_TRACE(m_pLogger, L"Starting graphics subsystem");

		m_pGraphics = new GraphicsLayer(m_pLogger);
		GraphicsLayer* graphics = (GraphicsLayer*)m_pGraphics;

		if(!graphics->Initialize(config))
		{
			AELOG_FATAL(m_pLogger, L"Could not initialize graphics subsystem");
			m_pGraphics->Shutdown();
			delete m_pGraphics;
			m_pGraphics = nullptr;
			return AFALSE;
		}

		m_vLiveSubSystems.push_back(m_pGraphics);
		AELOG_DEBUG(m_pLogger, L"Graphics subsystem initialized successfully");
		return ATRUE;
	}

	ABOOL EngineKernel::StartInputSubsystem(SubsystemConfiguration const & config)
	{
		m_pInput = new DirectInput(m_pLogger);
		DirectInput *input = (DirectInput*)m_pInput;

		if(!input->Initialize(config))
		{
			AELOG_FATAL(m_pLogger, L"Could not initialize input subsystem");			
			m_pInput->Shutdown();
			delete m_pInput;
			m_pInput = nullptr;
			return AFALSE;
		}

		m_vLiveSubSystems.push_back(m_pInput);
		AELOG_DEBUG(m_pLogger, L"Input system initialized successfully");
		return ATRUE;
	}

	void EngineKernel::StopSubsystems()
	{
		std::for_each(m_vLiveSubSystems.begin(), m_vLiveSubSystems.end(),
			[](Subsystem *system)
			{
				system->Shutdown();
				delete system;
			}
		);
		m_vLiveSubSystems.clear();
	}

	void EngineKernel::Shutdown()
	{
		AELOG_TRACE(m_pLogger, "Shutting down kernel...");
		StopSubsystems();
		DestroyWindow(m_config.hWnd);
		m_config.hWnd = 0;
		m_dt = 0;
		m_bExit = false;
		AELOG_TRACE(m_pLogger, "Kernel shutdown complete.");
	}

	void EngineKernel::HandleInput()
	{
		DirectInput *input = (DirectInput*)m_pInput;
		if(input->IsControl(AEINPUT_GAMECTRL_KEYBOARD | AEINPUT_CTRLACTION_PRESS | DIK_ESCAPE))
		{
			m_bExit = true;
		}
	}

	//safe way to get kernel to stop
	void EngineKernel::Stop()
	{
		m_bExit = true;
	}

	AINT32 EngineKernel::Run()
	{
		AELOG_INFO(m_pLogger, "Running kernel....");

		MSG msg;
		AINT64 prevTimeStamp = 0;
		AINT64 currTimeStamp;
		QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
		AFLOAT32 dt = 0;

		//grab initial context from configuration
		m_pRunningContext = m_config.pInitialContext;
		//call start on it
		m_pRunningContext->Start();
		//loop until engine knows it's time to exit
		while(!m_bExit)
		{	//handle Windows messages to be friendly to operating system
			if(PeekMessage(&msg, m_config.hWnd, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{	//No OS message -> time to process kernel update
				//start by recording time since last engine update
				QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
				dt = Clock::CyclesToSeconds(currTimeStamp - prevTimeStamp);
				m_clock.Update(dt);
				//get interface to input
				DirectInput* input = (DirectInput *)m_pInput;
				//poll input devices
				input->PollInputDevices();
				HandleInput();

				//call start frame for process managers
				m_kernelProcessMgr.StartFrame();
				m_userProcessMgr.StartFrame();
				//call update on running context
				m_pRunningContext->PreUpdate(dt);
				//allow processes to pre-update
				m_kernelProcessMgr.PreUpdateAll(dt);
				m_userProcessMgr.PreUpdateAll(dt);
				//call update on running context
				m_pRunningContext->Update(dt);
				//call post-update on processes
				m_kernelProcessMgr.PostUpdateAll(dt);
				m_userProcessMgr.PostUpdateAll(dt);
				//call post-update on context
				m_pRunningContext->PostUpdate(dt);
				//call end frame for process managers
				m_kernelProcessMgr.EndFrame();
				m_userProcessMgr.EndFrame();

				//if there is a context awaiting to be switched in
				if(m_pNextRunningContext)
				{	//stop current context
					m_pRunningContext->Stop();
					//set current context to what is next
					m_pRunningContext = m_pNextRunningContext;
					//start next context
					m_pRunningContext->Start();
					//set next to null
					m_pNextRunningContext = nullptr;
				}
				//time keeping
				prevTimeStamp = currTimeStamp;
			}
		}

		//make sure context is stopped
		m_pRunningContext->Stop();
		AELOG_INFO(m_pLogger, "Kernel run complete.  Exiting with status 0 (NORMAL)");
		return AEKES_SUCCESS;
	}

	void EngineKernel::ResizeMainWindow(AUINT32 width, AUINT32 height)
	{
		//defer call to graphics layer
		GraphicsLayer *gl = (GraphicsLayer *)m_pGraphics;
		gl->ResizeWindow(width, height);
	}

	HWND EngineKernel::CreateMainWindow()
	{
		WNDCLASSEX		wcl;

		//fill out window class (basic behavior)
		wcl.cbSize = sizeof(WNDCLASSEX);
		wcl.lpszClassName = lpszWindowClassName;
		wcl.hInstance = m_config.hAppInst;
		wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcl.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
		wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcl.style = 0;
		wcl.lpszMenuName = NULL;
		wcl.cbClsExtra = 0;
		wcl.cbWndExtra = 0;
		wcl.lpfnWndProc = WndProc;

		if(!RegisterClassEx(&wcl))
		{
			//show message box on error
			MessageBox(NULL, L"Error registering class!!!", L"Fatal Error", MB_OK | MB_ICONEXCLAMATION);
			AELOG_ERROR(m_pLogger, L"Could not register window class.  Exiting...");
			return NULL;
		}

		HWND hwnd;
		//window creation depends on fullscreen mode or not
		if(m_config.bFullscreen)
		{
			AELOG_TRACE(m_pLogger, L"Creating fullscreen window.");
			hwnd = CreateWindow(
				lpszWindowClassName,		//give it the window classname
				m_config.windowTitle,	//visible window title
				WS_POPUP | WS_VISIBLE,	//pop up window has no border/menu/style, create visible by default
				0, 0,			//top left at 0, 0
				GetSystemMetrics(SM_CXSCREEN),		//set width to width of current resolution
				GetSystemMetrics(SM_CYSCREEN),		//set height to height of current resolution
				HWND_DESKTOP,		//handle to parent window is desktop
				NULL,				//no menu associated with this window
				m_config.hAppInst,			//instance of application
				0 );				//lParam to pass to WndProc on WM_CREATE
		}
		else
		{
			AELOG_TRACE(m_pLogger, L"Creating non-fullscreen window.");
			//Adjust window rectangle so the actual 
			//rendering space is the resolution we set
			RECT r = {0, 0, m_config.windowWidth, m_config.windowHeight};
			AdjustWindowRect(&r, WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE, false);
			
			hwnd = CreateWindow(
				lpszWindowClassName,		//give it the window classname
				m_config.windowTitle,	//visible window title
				WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,	//create window with standard boder and buttons, create visible by default
				0, 0,		//top left at 0, 0
				r.right - r.left,			//set width
				r.bottom - r.top,			//set height
				HWND_DESKTOP,		//handle to parent window is desktop
				NULL,				//no menu associated with this window
				m_config.hAppInst,			//instance of application
				0 );				//lParam to pass to WndProc on WM_CREATE		
		}

		if(!hwnd)
		{
			//show message box
			MessageBox(NULL, L"Error creating handle to window!!!", L"Fatal Error", MB_OK | MB_ICONEXCLAMATION);
			return NULL;
		}

		//copy to global for use in DirectInput callback functions
		ShowWindow(hwnd, m_config.nCmdShow);
		UpdateWindow(hwnd);

		return hwnd;
	}

	//static window procedure
	LRESULT CALLBACK EngineKernel::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//Get kernel statically
		EngineKernel *kernel = EngineKernel::GetKernel();

		switch(msg)
		{
			/*
		case WM_ACTIVATE:		//we could pause the application if it loses focus here
			if( LOWORD(wParam) == WA_INACTIVE )
				;
			else
				;
			break;
			*/
			/*
		case WM_SIZE:
			if(kernel)
			{
				RECT windowRect;
				GetClientRect(kernel->m_hWnd, &windowRect);
				kernel->ResizeMainWindow(windowRect.right - windowRect.left,
										windowRect.bottom - windowRect.top);
			}
			break;
			*/
		case WM_CLOSE:
			if(kernel)
			{
				kernel->m_bExit = true;
				PostQuitMessage(0);
			}
			break;
		default:
			//any messages we don't handle, give back to Windows
			return DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
		return 0;
	}
}	//namespace Accevo