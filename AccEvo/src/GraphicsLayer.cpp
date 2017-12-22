
#include <iostream>
#include "GraphicsLayer.h"
#include <set>
#include <DirectXMath.h>
#include "DXUtil.h"

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

#define AE_LOG_OUTPUT_LEVEL AE_LOG_OUTPUT_TRACE

using namespace boost::property_tree;

namespace Accevo
{
	GraphicsLayer::GraphicsLayer(Logger *pLogger) :
		m_pLogger(pLogger),
		m_pDevice(nullptr),
		m_pImmediateDeviceContext(nullptr),
		m_pDXGIFactory(nullptr),
		m_pSwapChain(nullptr),
		m_isInitialized(false),
		m_vGraphicsDevices(0),
		m_backBuffer(nullptr),
		m_depthStencilBuffer(nullptr),
		m_pInfoQueue(nullptr),
		m_pD3DDebug(nullptr),
		m_graphicsResourceMgr(m_pLogger),
		m_graphicsBundles(m_pLogger),
		m_pRasterizerState(nullptr),
		m_pDepthStencilState(nullptr),
		m_pDxgiBackBufferSurface(nullptr),
		m_pd2dFactory(nullptr),
		m_pd2dDevice(nullptr),
		m_pDXGIDevice(nullptr),
		m_pDXGIAdapter(nullptr),
		m_pd2dDeviceContext(nullptr),
		m_pSolidBlueBrush(nullptr)
	{
		
	}

	GraphicsLayer::~GraphicsLayer()
	{
		if(IsInitialized())
		{
			AELOG_WARN(m_pLogger, L"Graphics layer not shutdown before deletion.  Shutting down now...");
			Shutdown();
		}
	}

	//initializes graphics subsystem for rendering to proceed
	bool GraphicsLayer::Initialize(SubsystemConfiguration const & config)
	{
		AELOG_DEBUG(m_pLogger, L"Initializing graphics layer");
		if(m_isInitialized)
		{
			AELOG_WARN(m_pLogger, L"Attempted to initialize already initialized graphics subsystem.");
			return false;
		}

		if(!CreateDeviceAndDXGIFactory(config))
		{
			AELOG_ERROR(m_pLogger, L"Could not initialize DXGIFactory or create DirectX11 device.");
			Shutdown();
			return m_isInitialized = false;
		}
		
		if(!EnumerateGraphicsInfo(config))
		{
			AELOG_ERROR(m_pLogger, L"Could not enumerate graphics configuration.");
			Shutdown();
			return m_isInitialized = false;
		}

		if(!GetGraphicsConfiguration(config))
		{
			AELOG_ERROR(m_pLogger, L"Could not find suitable graphics configuration.");
			Shutdown();
			return m_isInitialized = false;
		}
		
		if(!InitSwapChainAndMainBuffers())
		{
			AELOG_ERROR(m_pLogger, L"Could not create swap chain for graphics.");
			Shutdown();
			return m_isInitialized = false;
		}

		//OutputDebugString(L"OPEN -- OUTPUTTING LIVE OBJECTS\n");
		//m_pD3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
		//OutputDebugString(L"OPEN -- WALL OUTPUT LIVE OBJECTS\n");

		ID3D11RenderTargetView *pBackBufferRTV = m_backBuffer->GetRenderTargetView();
		m_pImmediateDeviceContext->OMSetRenderTargets(1, &pBackBufferRTV, m_depthStencilBuffer->GetDepthStencilView());

		D3D11_RASTERIZER_DESC rDesc;
		ZeroMemory(&rDesc, sizeof(D3D11_RASTERIZER_DESC));
		rDesc.AntialiasedLineEnable = false;
		rDesc.CullMode = D3D11_CULL_NONE;
		rDesc.DepthBias = 0;
		rDesc.DepthBiasClamp = 0.0f;
		rDesc.DepthClipEnable = false;
		rDesc.FillMode = D3D11_FILL_SOLID;
		rDesc.FrontCounterClockwise = false;
		rDesc.MultisampleEnable = false;
		rDesc.ScissorEnable = false;
		rDesc.SlopeScaledDepthBias = 0.0f;

		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not create rasterizer state!!!", m_pDevice->CreateRasterizerState(&rDesc, &m_pRasterizerState),
			return m_isInitialized = false;
		);
		m_pImmediateDeviceContext->RSSetState(m_pRasterizerState);

#ifdef _DEBUG_NOT	//prevents PIX from working
		DX_SET_DEBUG_NAME(m_pDevice, "m_pDevice");
		DX_SET_DEBUG_NAME(m_pImmediateDeviceContext, "m_pImmCtx");
		DX_SET_DEBUG_NAME(m_pSwapChain, "m_pSwapChain");
		DX_SET_DEBUG_NAME(m_backBuffer->GetResource(), "m_pBackBufferTEX");
		DX_SET_DEBUG_NAME(m_backBuffer->GetRenderTargetView(), "m_pBackBufferRTV");
		DX_SET_DEBUG_NAME(m_depthStencilBuffer->GetResource(), "m_pDepthStencilTEX");
		DX_SET_DEBUG_NAME(m_depthStencilBuffer->GetDepthStencilView(), "m_pDepthStencilDSV");
		DX_SET_DEBUG_NAME(m_pDXGIFactory, "m_pDXGIFactory");
#endif
		return m_isInitialized = true;
	}

	bool GraphicsLayer::EnumerateGraphicsInfo(SubsystemConfiguration const & config)
	{
		HRESULT hr;
		IDXGIAdapter1 *pAdapter = nullptr;
		unsigned int adapterIndex = 0;
		while(m_pDXGIFactory->EnumAdapters1(adapterIndex++, &pAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			GraphicsDevice *gd = new GraphicsDevice();
			m_vGraphicsDevices.push_back(gd);
			gd->pAdapter = pAdapter;
		
			IDXGIOutput				*pMonitor;
			unsigned int monitorIndex = 0;
			while(pAdapter->EnumOutputs(monitorIndex++, &pMonitor) != DXGI_ERROR_NOT_FOUND)
			{
				Monitor *monitor = new Monitor();
				monitor->pOutputHandle = pMonitor;
				gd->pvMonitors.push_back(monitor);
			
				//get number of display modes for monitor
				hr = pMonitor->GetDisplayModeList(config.pGraphicsConfigRequest->dxgiFormat, 0, &monitor->numDisplayModes, NULL);
				AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get display mode list for monitor", hr, continue;);
				//create vector large enough to hold data for all
				monitor->pDisplayModes = boost::shared_array<DXGI_MODE_DESC>(
					new DXGI_MODE_DESC[monitor->numDisplayModes]);
				//get actual information for display modes
				hr = pMonitor->GetDisplayModeList(config.pGraphicsConfigRequest->dxgiFormat, 0,
					&monitor->numDisplayModes, monitor->pDisplayModes.get());
				AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not enumerate display modes of monitor!!!", hr, return false);
			}
		}
		return true;
	}

	bool GraphicsLayer::CreateDeviceAndDXGIFactory(SubsystemConfiguration const & gcRequested)
	{
		HRESULT hr;
		AELOG_TRACE(m_pLogger, L"Creating DirectX11 device and obtaining DXGI factory...");
		//only request DirectX 11 or fail creation
		D3D_FEATURE_LEVEL requestedLevels[] = {
			D3D_FEATURE_LEVEL_11_1
			//D3D_FEATURE_LEVEL_10_1,
			//D3D_FEATURE_LEVEL_10_0,
			//D3D_FEATURE_LEVEL_9_3,
			//D3D_FEATURE_LEVEL_9_2,
			//D3D_FEATURE_LEVEL_9_1,
		};

		UINT d3dFlags = 0;
		d3dFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	#ifdef _DEBUG
		d3dFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

		D3D_FEATURE_LEVEL levelAchieved;
		hr = D3D11CreateDevice(
			//m_vGraphicsDevices[gcRequested.adapterIndex].pAdapter,
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,			//what kind of driver to create (we obviously want hardware)
			NULL,								//module handle to software rasterizer DLL if we have one
			d3dFlags,							//no special flags for this device (debug, threading behavior, etc)
			requestedLevels, 1,					//array of feature levels to ask for and number of feature levels in array
			//NULL, NULL,					//array of feature levels to ask for and number of feature levels in array
			D3D11_SDK_VERSION,					//D3D SDK version being used to develop
			&m_pDevice,							//pointer to device out
			&levelAchieved,						//pointer to a D3D_FEATURE_LEVEL to output achieved level to
			&m_pImmediateDeviceContext);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not create DirectX11 device!!!", hr, return false);
	

		IDXGIDevice * pDXGIDevice;
		hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get DXGI device from D3D11 device!!!", hr, return false);

		IDXGIAdapter1 * pDXGIAdapter;
		hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter1), (void **)&pDXGIAdapter);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get DXGI adapter from DXGI device!!!", hr, return false);

		hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory1), (void **)&m_pDXGIFactory);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get DXGI factory from DXGI adapter!!!", hr, return false);

		D2D1_FACTORY_OPTIONS d2dFactoryOpts;
		d2dFactoryOpts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory6), &d2dFactoryOpts, (void **)&m_pd2dFactory);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not create D2D factory", hr, return false);

		/*
		hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice4), (void **)&m_pDXGIDevice);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get DXGI device from D3D11 device!!!", hr, return false);

		hr = m_pDXGIDevice->GetParent(__uuidof(IDXGIAdapter4), (void **)&m_pDXGIAdapter);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get DXGI adapter from DXGI device!!!", hr, return false);

		hr = m_pDXGIDevice->GetParent(__uuidof(IDXGIFactory4), (void **)&m_pDXGIFactory);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get DXGI factory from DXGI adapter!!!", hr, return false);
		*/

		//get extra interfaces to get information
		m_pDevice->QueryInterface(__uuidof(ID3D11Debug), (void **)&m_pD3DDebug);
		m_pDevice->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&m_pInfoQueue);

		AELOG_TRACE(m_pLogger, L"DirectX11 device created and DXGIFactory obtained successfully.");
		return true;
	}

	bool GraphicsLayer::GetGraphicsConfiguration(SubsystemConfiguration const & config)
	{
		AELOG_TRACE(m_pLogger, L"Finding appropriate graphics mode match");
		m_graphicsConfig.bFullscreen = config.pKernelConfig->bFullscreen;
		m_graphicsConfig.hwnd = config.pKernelConfig->hWnd;
		m_graphicsConfig.bVSync = config.pKernelConfig->bVsync;
		//Get all elements of requested configuration
		m_graphicsConfig.device = m_vGraphicsDevices.at(
			config.pGraphicsConfigRequest->adapterIndex);
		m_graphicsConfig.monitor = m_graphicsConfig.device.pvMonitors.at(
			config.pGraphicsConfigRequest->monitorIndex);
	
		std::set<DXGI_MODE_DESC *, Monitor::ProgRefreshScalingModeCmp> matchedModes;

		//collect all matching modes and sort by some comparison method
		for(unsigned int modeIndex = 0; modeIndex < m_graphicsConfig.monitor.numDisplayModes; ++modeIndex)
		{
			DXGI_MODE_DESC &mode = m_graphicsConfig.monitor.pDisplayModes[modeIndex];
			if(mode.Format == config.pGraphicsConfigRequest->dxgiFormat &&
				mode.Width == config.pGraphicsConfigRequest->backBufferWidth &&
				mode.Height == config.pGraphicsConfigRequest->backBufferHeight)
			{
				matchedModes.insert(&mode);
			}
		}
		if(!matchedModes.empty())
		{
			m_graphicsConfig.mode = **matchedModes.rbegin();
			AELOG_TRACE(m_pLogger, L"Match found");
			return true;
		}
		AELOG_WARN(m_pLogger, L"Graphics mode match not found");
		return false;
	}

	//updates the graphics layer
	void GraphicsLayer::Update(float dt)
	{
		std::wostringstream sstream;
		UINT64 nMsg;
		if(nMsg = m_pInfoQueue->GetNumStoredMessages())
		{
			sstream << "Found " << nMsg << " messages.";
			AELOG_INFO(m_pLogger, sstream.str().c_str());
			sstream.clear();
			D3D11_MESSAGE *msg;
			SIZE_T msgLength;
			for(unsigned __int64 msgIndex = 0; msgIndex < nMsg; ++msgIndex)
			{	
				msgLength = 0;
				m_pInfoQueue->GetMessage(msgIndex, NULL, &msgLength);
				boost::shared_array<char> msgMemory(new char[msgLength]);
				msg = (D3D11_MESSAGE *)msgMemory.get();
				m_pInfoQueue->GetMessage(msgIndex, msg, &msgLength);
				
				sstream << "DirectX11 InfoQueue Message: " << msg->pDescription;
				AELOG_INFO(m_pLogger, sstream.str().c_str());
				sstream.clear();
			}

			m_pInfoQueue->ClearStoredMessages();
		}
	}

	bool GraphicsLayer::InitSwapChainAndMainBuffers()
	{
		HRESULT hr;
		AELOG_TRACE(m_pLogger, L"Creating swap chain...");
		DXGI_SWAP_CHAIN_DESC scDesc;
		ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		scDesc.BufferCount = 1;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.BufferDesc.Format = m_graphicsConfig.mode.Format;	
		scDesc.BufferDesc.Width = m_graphicsConfig.mode.Width;
		scDesc.BufferDesc.Height = m_graphicsConfig.mode.Height;
		scDesc.BufferDesc.RefreshRate.Numerator = m_graphicsConfig.mode.RefreshRate.Numerator;
		scDesc.BufferDesc.RefreshRate.Denominator = m_graphicsConfig.mode.RefreshRate.Denominator;
		scDesc.BufferDesc.Scaling = m_graphicsConfig.mode.Scaling;
		scDesc.BufferDesc.ScanlineOrdering = m_graphicsConfig.mode.ScanlineOrdering;
		scDesc.SampleDesc.Count = 1;		//one sample per pixel
		scDesc.SampleDesc.Quality = 0;		//regular quality
		scDesc.OutputWindow = m_graphicsConfig.hwnd;
		scDesc.Windowed = !m_graphicsConfig.bFullscreen;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		//if going to fullscreen, keep the resolution by forcing the desktop
		//to change if necessary.  Do not create front/back buffers with the
		//same resolution as desktop setting
		scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		hr = m_pDXGIFactory->CreateSwapChain(m_pDevice, &scDesc, &m_pSwapChain);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not create swap chain!!!", hr, return false);

		hr = m_pDXGIFactory->MakeWindowAssociation(m_graphicsConfig.hwnd, 0);
			//DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
		AELOG_DXERR_CONDITIONAL_DEBUG(m_pLogger, "MakeWindowAssociation: ", hr);

		/*
		if(m_graphicsConfig.bFullscreen)
		{
			AELOG_DEBUG(m_pLogger, L"Switching to fullscreen mode.");
			m_pSwapChain->SetFullscreenState(TRUE, NULL);
		}
		*/

		/*
		hr = m_pDXGIFactory->MakeWindowAssociation(m_graphicsConfig.hwnd,
			DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not make window association with DXGI to prevent alt-enter sequence",
			hr, return false);
		*/

		if(!InitMainBuffers())
		{
			AELOG_ERROR(m_pLogger, "Could not initialize main buffers!");
			return false;
		}

		AELOG_TRACE(m_pLogger, L"Swap chain and buffers created successfully.");
		return true;
	}

	bool GraphicsLayer::GetBackBufferGraphicsSurface()
	{
		HRESULT hr;
		ID3D11Texture2D *pBackBufferTexture = nullptr;
		//now get the pointer to the back buffer and create render target view from it
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&pBackBufferTexture);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get back buffer texture!!!", hr, return false);

		//now get the pointer to the back buffer and create IDXGISurface to it
		hr = m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void **)&m_pDxgiBackBufferSurface);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get back buffer DXGI surface!!!", hr, return false);

		D2D1_RENDER_TARGET_PROPERTIES rtp;
		memset(&rtp, 0, sizeof(D2D1_RENDER_TARGET_PROPERTIES));
		m_pd2dFactory->GetDesktopDpi(&rtp.dpiX, &rtp.dpiY);
		rtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		rtp.pixelFormat.format = m_graphicsConfig.mode.Format;
		rtp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
		hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(m_pDxgiBackBufferSurface, rtp, &m_pd2dRenderTarget);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get back buffer D2D render target!!!", hr, return false);

		m_pd2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.f, 0.f, 1.f), &m_pSolidBlueBrush);

		//doesnt release
		hr = m_pd2dRenderTarget->QueryInterface(__uuidof(ID2D1DeviceContext), (void **)&m_pd2dDeviceContext);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not acquire D2D1 device context", hr, return false);
		

		m_backBuffer = new GraphicsResource(m_pLogger, m_pDevice, pBackBufferTexture, 
			D3D11_BIND_RENDER_TARGET,
			nullptr, nullptr, nullptr, nullptr);
		ID3D11RenderTargetView *pRTV = m_backBuffer->GetRenderTargetView();
		if(!pRTV)
		{
			AELOG_INFO(m_pLogger, L"Could not get render target view of back buffer");
			return false;
		}
		return true;
	}

	bool GraphicsLayer::CreateDepthStencilSurface()
	{
		HRESULT hr;
		D3D11_TEXTURE2D_DESC dbTextureDesc;		
		D3D11_TEXTURE2D_DESC backBufferDesc;

		((ID3D11Texture2D *)m_backBuffer->GetResource())->GetDesc(&backBufferDesc);

		//Set up the description of the depth buffer
		dbTextureDesc.Width = backBufferDesc.Width;
		dbTextureDesc.Height = backBufferDesc.Height;
		dbTextureDesc.MipLevels = 1;
		dbTextureDesc.ArraySize = 1;
		dbTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dbTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dbTextureDesc.SampleDesc.Count = 1;
		dbTextureDesc.SampleDesc.Quality = 0;
		dbTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		dbTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dbTextureDesc.CPUAccessFlags = 0;
		dbTextureDesc.MiscFlags = 0;
	
		ID3D11Texture2D *pDepthBufferTexture = nullptr;
		hr = m_pDevice->CreateTexture2D(&dbTextureDesc, NULL, &pDepthBufferTexture);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not create texture for depth buffer!!!", hr, return false);

		m_depthStencilBuffer = new GraphicsResource(m_pLogger, m_pDevice, pDepthBufferTexture,
			D3D11_BIND_DEPTH_STENCIL,
			NULL,
			NULL,	//change
			NULL,
			NULL);

		D3D11_DEPTH_STENCIL_DESC dsSDesc;
		ZeroMemory(&dsSDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		 //Set up the description of the stencil state.
		dsSDesc.DepthEnable = true;
		dsSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsSDesc.DepthFunc = D3D11_COMPARISON_LESS;

		dsSDesc.StencilEnable = true;
		dsSDesc.StencilReadMask = 0xFF;
		dsSDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		dsSDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsSDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		dsSDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsSDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsSDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsSDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not create a depth stencil state.", m_pDevice->CreateDepthStencilState(&dsSDesc, &m_pDepthStencilState),
			return false;);

		m_pImmediateDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

		return true;
	}

	bool GraphicsLayer::SetupViewport()
	{
		D3D11_VIEWPORT vpDesc;
		vpDesc.Width = static_cast<float>(m_graphicsConfig.mode.Width);
		vpDesc.Height = static_cast<float>(m_graphicsConfig.mode.Height);
		vpDesc.MinDepth = 0.0f;
		vpDesc.MaxDepth = 1.0f;
		vpDesc.TopLeftX = 0.0f;
		vpDesc.TopLeftY = 0.0f;

		m_pImmediateDeviceContext->RSSetViewports(1, &vpDesc);
		
		return true;
	}

	//clears the backbuffer to a specified color
	void GraphicsLayer::Clear(float color[4])
	{
		//clear back buffer
		m_pImmediateDeviceContext->ClearRenderTargetView(
			m_backBuffer->GetRenderTargetView(),
			color);
		//clear depth-stencil
		m_pImmediateDeviceContext->ClearDepthStencilView(m_depthStencilBuffer->GetDepthStencilView(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f, 0);
	}

	void GraphicsLayer::Present()
	{
		m_pd2dRenderTarget->BeginDraw();

		m_pd2dRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pd2dRenderTarget->DrawLine(D2D1::Point2F(10.f, 10.f), D2D1::Point2F(1910.f, 1070.f), 
			m_pSolidBlueBrush, 1.f, nullptr);

		m_pd2dRenderTarget->EndDraw();

		m_pSwapChain->Present(m_graphicsConfig.bVSync? 1 : 0, 0);
	}

	bool GraphicsLayer::InitMainBuffers()
	{
		AELOG_DEBUG(m_pLogger, "Initializing main buffers");
		if(!GetBackBufferGraphicsSurface())
		{
			AELOG_ERROR(m_pLogger, L"Could not get back buffer surface from swap chain.");
			return false;
		}

		if(!CreateDepthStencilSurface())
		{
			AELOG_ERROR(m_pLogger, L"Could not create depth stencil surface for back buffer");
			return false;
		}
	
		m_pImmediateDeviceContext->OMSetRenderTargets(1, &m_backBuffer->GetRenderTargetView(), m_depthStencilBuffer->GetDepthStencilView());

		if(!SetupViewport())
		{
			AELOG_ERROR(m_pLogger, L"Could not set up viewport for primary window.");
			return false;
		}
		AELOG_TRACE(m_pLogger, "Successfully initialized main buffers");

		return true;
	}

	void GraphicsLayer::ReleaseMainBuffers()
	{
		AELOG_DEBUG(m_pLogger, "Releasing main buffers...");
		if(m_pImmediateDeviceContext)
		{
			m_pImmediateDeviceContext->OMSetRenderTargets(0, NULL, NULL);
		}

		if(m_backBuffer)
		{
			delete m_backBuffer;
			m_backBuffer = nullptr;
		}
		DX_RELEASE(m_pDxgiBackBufferSurface);
		DX_RELEASE(m_pDepthStencilState);
		if(m_depthStencilBuffer)
		{
			delete m_depthStencilBuffer;
			m_depthStencilBuffer = nullptr;
		}
	}

	bool GraphicsLayer::ResizeWindow(unsigned int width, unsigned int height)
	{
		AELOG_DEBUG(m_pLogger, (boost::format("Resizing window associated buffers to: (%1%, %2%)") % width %height).str().c_str());
		
		ReleaseMainBuffers();

		//release swap chain related elements
		m_graphicsConfig.mode.Width = width;
		m_graphicsConfig.mode.Height = height;
		m_pSwapChain->ResizeBuffers(1, width, height, m_graphicsConfig.mode.Format, 0);
		
		if(!InitMainBuffers())
		{
			AELOG_ERROR(m_pLogger, "Could not recreate main buffers for resize!");
			return false;
		}
		
		AELOG_TRACE(m_pLogger, "Done resizing window");
		return true;
	}

	void GraphicsLayer::ProcessMessageQueue()
	{
		UINT64 nMsg;
		if(nMsg = m_pInfoQueue->GetNumStoredMessages())
		{
			AELOG_DEBUG(m_pLogger, (boost::format("Found %1% messages")%nMsg).str().c_str());
			D3D11_MESSAGE *msg;
			SIZE_T msgLength;
			for(unsigned __int64 msgIndex = 0; msgIndex < nMsg; ++msgIndex)
			{	
				msgLength = 0;
				m_pInfoQueue->GetMessage(msgIndex, 0, &msgLength);
				boost::shared_array<char> msgMemory(new char[msgLength]);
				msg = (D3D11_MESSAGE *)msgMemory.get();
				m_pInfoQueue->GetMessage(msgIndex, msg, &msgLength);
				AELOG_DEBUG(m_pLogger, (boost::format("Message: %1%")%msg->pDescription).str().c_str());				
			}
			m_pInfoQueue->ClearStoredMessages();
		}
	}

	void GraphicsLayer::ReleaseSwapChainAndMainBuffers()
	{
		HRESULT hr;
		AELOG_DEBUG(m_pLogger, "Releasing swap chain and main buffers");
		ReleaseMainBuffers();

		//must unfullscreen before being able to release swap chain
		if(m_pSwapChain)
		{
			BOOL isFullscreen;
			IDXGIOutput *pOutput;
			hr = m_pSwapChain->GetFullscreenState(&isFullscreen, &pOutput);
			AELOG_DXERR_CONDITIONAL_ERROR(m_pLogger, "Could not obtain full screen state of swap chain!!!", hr);
			if(isFullscreen)
			{
				AELOG_TRACE(m_pLogger, "Application found in fullscreen, setting back to windowed.");
				hr = m_pSwapChain->SetFullscreenState(FALSE, nullptr);
				AELOG_DXERR_DEBUG(m_pLogger, "Switch to windowed mode: ", hr);
				pOutput->Release();
			}
			DX_RELEASE(m_pSwapChain);
		}
	}

	//shuts down the graphics subsystem by cleaning up everything associated with it
	void GraphicsLayer::Shutdown()
	{
		AELOG_TRACE(m_pLogger, L"Graphics subsystem shutting down...");

		for(unsigned int adapterIndex = 0; adapterIndex < m_vGraphicsDevices.size(); ++adapterIndex)
		{
			for(unsigned int monitorIndex = 0; monitorIndex < m_vGraphicsDevices.at(adapterIndex).pvMonitors.size(); ++monitorIndex)
			{
				DX_RELEASE(m_vGraphicsDevices.at(adapterIndex).pvMonitors.at(monitorIndex).pOutputHandle);
			}
			DX_RELEASE(m_vGraphicsDevices.at(adapterIndex).pAdapter);
		}

		DX_RELEASE(m_pRasterizerState);

		ReleaseSwapChainAndMainBuffers();

		DX_RELEASE(m_pSolidBlueBrush);
		DX_RELEASE(m_pd2dRenderTarget);
		DX_RELEASE(m_pd2dDeviceContext);
		DX_RELEASE(m_pd2dFactory);
		DX_RELEASE(m_pDXGIFactory);

		if(m_pInfoQueue)
		{
			ProcessMessageQueue();
		}

		DX_RELEASE(m_pD3DDebug);
		DX_RELEASE(m_pInfoQueue);
		if(m_pImmediateDeviceContext)
		{
			m_pImmediateDeviceContext->ClearState();
		}
		DX_RELEASE(m_pDXGIAdapter);
		DX_RELEASE(m_pDXGIDevice);

		DX_RELEASE(m_pImmediateDeviceContext);
		DX_RELEASE(m_pDevice);

		m_isInitialized = false;
		AELOG_TRACE(m_pLogger, L"Graphics subsystem shutdown complete.");
	}

	GraphicsResource * GraphicsLayer::LoadTexture(wchar_t const *filename)
	{
		/*
		HRESULT hr;
		boost::optional<D3DX11_IMAGE_LOAD_INFO &> imgLoadInfo;

		Handle hImgLoad;	//TODO: change //imageLoadInfoMgr.GetHandle(HashString("defaultImageLoad"));
		imgLoadInfo = m_imageLoadInfoMgr.GetDataOptional(hImgLoad);

		imgLoadInfo->BindFlags = 0;
		imgLoadInfo->CpuAccessFlags = D3DX11_DEFAULT;
		imgLoadInfo->Depth = D3DX11_DEFAULT;
		imgLoadInfo->Width = D3DX11_DEFAULT;
		imgLoadInfo->Height = D3DX11_DEFAULT;
		imgLoadInfo->Filter = D3DX11_DEFAULT;
		imgLoadInfo->FirstMipLevel = D3DX11_DEFAULT;
		imgLoadInfo->MipFilter = D3DX11_DEFAULT;
		imgLoadInfo->MipLevels = D3DX11_DEFAULT;
		imgLoadInfo->MiscFlags = D3DX11_DEFAULT;
		imgLoadInfo->pSrcInfo = nullptr;
		imgLoadInfo->Usage = D3D11_USAGE_DEFAULT;
		imgLoadInfo->Format = DXGI_FORMAT_UNKNOWN;
		ID3D11Resource *pResource = nullptr;

		hr = D3DX11CreateTextureFromFile(
			m_pDevice, filename, NULL, NULL, &pResource, NULL);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not load texture from file!!!", hr, return false);
		return new GraphicsResource(m_pLogger, m_pDevice, pResource,
			imgLoadInfo->BindFlags,
			nullptr, nullptr, nullptr, nullptr);

			*/
		return nullptr;	//TODO!! how to load textures!
	}
}		//namespace Accevo
