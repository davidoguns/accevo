//David Oguns
//February 1, 2011
//GraphicsResource.cpp

#include "GraphicsResource.h"
#include "DXUtil.h"

namespace Accevo
{
	GraphicsResource::GraphicsResource(ID3D11Device *pDevice,
			ID3D11Resource *pResource,
			UINT bindFlags,
			D3D11_RENDER_TARGET_VIEW_DESC const * renderTargetDesc,
			D3D11_DEPTH_STENCIL_VIEW_DESC const * depthStencilDesc, 
			D3D11_SHADER_RESOURCE_VIEW_DESC const * shaderResourceDesc,
			D3D11_UNORDERED_ACCESS_VIEW_DESC const * unorderedAccessDesc)  :
		m_pResource(pResource),
		m_bindFlags(bindFlags),
		m_pRenderTargetView(nullptr),
		m_pDepthStencilView(nullptr),
		m_pShaderResourceView(nullptr),
		m_pUnorderedAccessView(nullptr)
	{
		Initialize(nullptr, 
			pDevice, 
			pResource,
			m_bindFlags,
			renderTargetDesc,
			depthStencilDesc, 
			shaderResourceDesc, 
			unorderedAccessDesc);
	}

	GraphicsResource::GraphicsResource(
			Logger *pLogger,
			ID3D11Device *pDevice,
			ID3D11Resource *pResource,
			UINT bindFlags,
			D3D11_RENDER_TARGET_VIEW_DESC const * renderTargetDesc,
			D3D11_DEPTH_STENCIL_VIEW_DESC const * depthStencilDesc, 
			D3D11_SHADER_RESOURCE_VIEW_DESC const * shaderResourceDesc,
			D3D11_UNORDERED_ACCESS_VIEW_DESC const * unorderedAccessDesc)  :
		m_pResource(pResource),
		m_bindFlags(bindFlags),
		m_pRenderTargetView(nullptr),
		m_pDepthStencilView(nullptr),
		m_pShaderResourceView(nullptr),
		m_pUnorderedAccessView(nullptr)
	{
		Initialize(nullptr, 
			pDevice, 
			pResource,
			m_bindFlags,
			renderTargetDesc,
			depthStencilDesc,
			shaderResourceDesc, 
			unorderedAccessDesc);
	}

	void GraphicsResource::Initialize(
			Logger *pLogger,
			ID3D11Device *pDevice,
			ID3D11Resource *pResource,
			UINT bindFlags,
			D3D11_RENDER_TARGET_VIEW_DESC const * renderTargetDesc,
			D3D11_DEPTH_STENCIL_VIEW_DESC const * depthStencilDesc, 
			D3D11_SHADER_RESOURCE_VIEW_DESC const * shaderResourceDesc,
			D3D11_UNORDERED_ACCESS_VIEW_DESC const * unorderedAccessDesc)
	{
		HRESULT hr;
		if(bindFlags & D3D11_BIND_RENDER_TARGET)
		{
			hr = pDevice->CreateRenderTargetView(m_pResource, renderTargetDesc, &m_pRenderTargetView);
			AELOG_DXERR_CONDITIONAL_ERROR(pLogger, L"Could not create render target view of graphics resource!!!", hr);
		}
		if(bindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			hr = pDevice->CreateDepthStencilView(m_pResource, depthStencilDesc, &m_pDepthStencilView);
			AELOG_DXERR_CONDITIONAL_ERROR(pLogger, L"Could not create depth-stencil view of graphics resource!!!", hr);
		}
		if(bindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			hr = pDevice->CreateShaderResourceView(m_pResource, shaderResourceDesc, &m_pShaderResourceView);
			AELOG_DXERR_CONDITIONAL_ERROR(pLogger, L"Could not create shader resource view of graphics resource!!!", hr);
		}
		if(bindFlags & D3D11_BIND_UNORDERED_ACCESS)
		{
			hr = pDevice->CreateUnorderedAccessView(m_pResource, unorderedAccessDesc, &m_pUnorderedAccessView);
			AELOG_DXERR_CONDITIONAL_ERROR(pLogger, L"Could not create unordered access view of graphics resource!!!", hr);
		}
	}

	GraphicsResource::~GraphicsResource()
	{
		DX_RELEASE(m_pRenderTargetView);
		DX_RELEASE(m_pDepthStencilView);
		DX_RELEASE(m_pShaderResourceView);
		DX_RELEASE(m_pUnorderedAccessView);
		DX_RELEASE(m_pResource);
	}
}
