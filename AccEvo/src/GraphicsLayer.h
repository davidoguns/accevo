//David Oguns
//December 26, 2010

#ifndef _GRAPHICSLAYER_H_
#define _GRAPHICSLAYER_H_

#include "GlobalInclude.h"
#include <D3D11.h>
#include <boost/shared_array.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "Subsystem.h"
#include "GraphicsResource.h"
#include "HandleManager.h"
#include "StringIdentifier.h"
#include "Logger.h"
#include <boost/property_tree/ptree.hpp>

namespace Accevo
{
	struct GraphicsBundle
	{
		//IDs of InputLayout interfaces
		std::list<StringIdentifier>		inputLayoutHandles;
		//IDs of image load info structure instances
		std::list<StringIdentifier>		imageLoadInfoDescHandles;
		//IDs of shader resource view descriptions
		std::list<StringIdentifier>		shaderResourceViewDescHandles;
		//IDs of texture resources
		std::list<StringIdentifier>		textureHandles;
	};

	class GraphicsLayer : public Subsystem
	{
	private:
		struct Monitor
		{
			IDXGIOutput							*pOutputHandle;		//DXGI pointer to output device
			boost::shared_array<DXGI_MODE_DESC>	pDisplayModes;		//vector of display modes
			UINT								numDisplayModes;	//how many display modes are populated
		
			/**
			 * Orders DXGI_MODE_DESCs in such a way that the Accevo defined
			 * preference appears first in a sorted list.  Makes it easy to prioritize.
			 */
			struct ProgRefreshScalingModeCmp
			{
				bool operator()(DXGI_MODE_DESC const *lhs, DXGI_MODE_DESC const *rhs)
				{
					if(lhs->ScanlineOrdering < rhs->ScanlineOrdering)
						return true;
					else if(lhs->ScanlineOrdering == rhs->ScanlineOrdering)
					{
						float lhsRR = float(lhs->RefreshRate.Numerator)/float(lhs->RefreshRate.Denominator);
						float rhsRR = float(rhs->RefreshRate.Numerator)/float(rhs->RefreshRate.Denominator);
						if(lhsRR < rhsRR)
							return true;
						else if(lhsRR == rhsRR)
							return lhs->Scaling < rhs->Scaling;
						return false;
					}
					return false;
				}
			};
		};

		struct GraphicsDevice
		{
			IDXGIAdapter1				*pAdapter;
			boost::ptr_vector<Monitor>	pvMonitors;
		};
	
		struct GraphicsConfiguration
		{
			GraphicsDevice		device;
			Monitor				monitor;
			DXGI_MODE_DESC		mode;
			bool				bVSync;
			HWND				hwnd;
			bool				bFullscreen;
		};

	public:
		GraphicsLayer(Logger *pLogger);

		GraphicsLayer(const GraphicsLayer &) = delete;
		GraphicsLayer& operator=(const GraphicsLayer &) = delete;

		virtual ~GraphicsLayer();

		//initializes graphics subsystem for rendering to proceed
		virtual bool Initialize(SubsystemConfiguration const & config);
		//used to check if graphics subsystem has initialized properly
		virtual bool IsInitialized() const { return m_isInitialized; }

		//updates the graphics layer
		virtual void Update(float dt);

		//clears the backbuffer to a specified color
		virtual void Clear(float color[4]);

		//present image to screen
		virtual void Present();

		//shuts down the graphics subsystem by cleaning up everything associated with it
		virtual void Shutdown();

		ID3D11Device * GetDevice() const { return m_pDevice; }
		ID3D11DeviceContext * GetImmediateDeviceContext() const { return m_pImmediateDeviceContext; }		
		
		bool ResizeWindow(unsigned int width, unsigned int height);

		unsigned int GetBackBufferWidth() const { return m_graphicsConfig.mode.Width; }
		unsigned int GetBackBufferHeight() const { return m_graphicsConfig.mode.Height; }

		/**************************************************************
			Resource management methods, support loading configuration
			from files instead of hardcoding population of D3D11 structs.
		****************************************************************/
		
		//loads texture to a surface and makes it minimally a shader resource
		GraphicsResource * LoadTexture(wchar_t const *filename);
		
	protected:
		bool GetGraphicsConfiguration(SubsystemConfiguration const & gcRequested);
		bool EnumerateGraphicsInfo(SubsystemConfiguration const & gcRequested);
		bool InitSwapChainAndMainBuffers();
		bool InitMainBuffers();
		void ReleaseSwapChainAndMainBuffers();
		void ReleaseMainBuffers();
		bool CreateDeviceAndDXGIFactory(SubsystemConfiguration const & gcRequested);
		bool GetBackBufferGraphicsSurface();
		bool CreateDepthStencilSurface();
		bool SetupViewport();
		bool SetRasterizerState();

		void ProcessMessageQueue();

	private:
		bool									m_isInitialized;				//flag indicating whether or not graphics layer is properly initialized
		Logger									*m_pLogger;						//logger

		boost::ptr_vector<GraphicsDevice>		m_vGraphicsDevices;			//holds information about all graphics devices during startup

		ID3D11Device					*m_pDevice;						//pointer to device
		ID3D11DeviceContext				*m_pImmediateDeviceContext;		//pointer to immediate device context
		IDXGIFactory1					*m_pDXGIFactory;				//factory to obtain graphics device layers
		IDXGISwapChain					*m_pSwapChain;					//pointer to swap chain to present to screen
		GraphicsResource				*m_backBuffer;					//back buffer surface
		GraphicsResource				*m_depthStencilBuffer;			//depth stencil buffer
		ID3D11RasterizerState			*m_pRasterizerState;
		ID3D11DepthStencilState			*m_pDepthStencilState;
		
		GraphicsConfiguration			m_graphicsConfig;				//acheived configuration
		ID3D11Debug						*m_pD3DDebug;			//debugging info
		ID3D11InfoQueue					*m_pInfoQueue;			//debugging info

		HandleManager<GraphicsBundle>			m_graphicsBundles;
		HandleManager<GraphicsResource *>		m_graphicsResourceMgr;
	};
}	//namespace Accevo

#endif
