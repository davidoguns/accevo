//David Oguns
//February 1, 2011

#pragma once

#include "GlobalInclude.h"
#include <D3D11.h>
#include "Logger.h"

namespace Accevo
{
	/************************************************************
		A graphics resource can be a vertex buffer, index buffer,
		any dimensional texture, etc.  This base class simply 
		groups any views associated with the resource with the
		resource itself to avoid duplication and consolidate
		resource freeing code.

		The views cannot be created in this base class because
		the type of resource must be known to get the full details.
		Views are to be constructed by the subclasses as appropriate.
		Using the provided members and a virtual destructor guarantees
		their cleanup upon destruction.
	*************************************************************/

	class GraphicsResource
	{
	public:
		GraphicsResource(GraphicsResource const &) = delete;
		GraphicsResource& operator=(GraphicsResource const &) = delete;

		GraphicsResource(ID3D11Device *pDevice,
			ID3D11Resource *pResource,
			UINT bindFlags,
			D3D11_RENDER_TARGET_VIEW_DESC const * renderTargetDesc,
			D3D11_DEPTH_STENCIL_VIEW_DESC const * depthStencilDesc, 
			D3D11_SHADER_RESOURCE_VIEW_DESC const * shaderResourceDesc,
			D3D11_UNORDERED_ACCESS_VIEW_DESC const * unorderedAccessDesc);

		GraphicsResource(
			Logger *pLogger,
			ID3D11Device *pDevice,
			ID3D11Resource *pResource,
			UINT bindFlags,
			D3D11_RENDER_TARGET_VIEW_DESC const * renderTargetDesc,
			D3D11_DEPTH_STENCIL_VIEW_DESC const * depthStencilDesc, 
			D3D11_SHADER_RESOURCE_VIEW_DESC const * shaderResourceDesc,
			D3D11_UNORDERED_ACCESS_VIEW_DESC const * unorderedAccessDesc);

		void Initialize(
			Logger *pLogger,
			ID3D11Device *pDevice,
			ID3D11Resource *pResource,
			UINT bindFlags,
			D3D11_RENDER_TARGET_VIEW_DESC const * renderTargetDesc,
			D3D11_DEPTH_STENCIL_VIEW_DESC const * depthStencilDesc, 
			D3D11_SHADER_RESOURCE_VIEW_DESC const * shaderResourceDesc,
			D3D11_UNORDERED_ACCESS_VIEW_DESC const * unorderedAccessDesc);

		virtual ~GraphicsResource();

		ID3D11Resource * const & GetResource() const { return m_pResource; }
		ID3D11RenderTargetView * const & GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView * const & GetDepthStencilView() const { return m_pDepthStencilView; }
		ID3D11ShaderResourceView * const & GetShaderResourceView() const { return m_pShaderResourceView; }
		ID3D11UnorderedAccessView * const & GetUnorderedAccessView() const { return m_pUnorderedAccessView; }

	protected:
		ID3D11Resource					*m_pResource;
		UINT							m_bindFlags;
		ID3D11RenderTargetView			*m_pRenderTargetView;
		ID3D11DepthStencilView			*m_pDepthStencilView;
		ID3D11ShaderResourceView		*m_pShaderResourceView;
		ID3D11UnorderedAccessView		*m_pUnorderedAccessView;
	};

}	//namespace Accevo
