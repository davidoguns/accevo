//David Oguns
//December 27, 2010

#include "Shader.h"
#include "aetypes.h"
#include "EngineKernel.h"
#include <boost/format.hpp>

using std::endl;
using boost::format;
using boost::shared_array;

namespace Accevo
{

Shader::Shader() :
	m_isInitialized(false),
	m_hr(E_FAIL),
	m_pShader(nullptr),
	m_nShaderResourceViews(0),
	m_nSamplerStates(0)
{
	ZeroMemory(&m_shaderDesc, sizeof(D3D11_SHADER_DESC));
}

Shader::~Shader()
{	
	for(AUINT32 cbIndex = 0; cbIndex < m_shaderDesc.ConstantBuffers; ++cbIndex)
	{
		DX_RELEASE(m_pConstantBuffers[cbIndex]);
	}
	for(AUINT32 srvIndex = 0; srvIndex < m_nShaderResourceViews; ++srvIndex)
	{
		DX_RELEASE(m_pShaderResourceViews[srvIndex]);
	}
	for(AUINT32 ssIndex = 0; ssIndex < m_nSamplerStates; ++ssIndex)
	{
		DX_RELEASE(m_pSamplerStates[ssIndex]);
	}

	DX_RELEASE(m_pShader);
}



//derives information from the shader loaded into the blob to initialize shader interface fully
//returns the shader reflector to the caller for derived classes to use further
ABOOL Shader::ReflectShader(ID3D11Device *pDevice, ID3D10Blob *vsBlob, ID3D11ShaderReflection* &pReflector)
{
	m_hr = D3DReflect( vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), 
           IID_ID3D11ShaderReflection, (void**) &pReflector);
	AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not create shader reflector from blob!!!", m_hr,
		return false);
	
	m_hr = pReflector->GetDesc(&m_shaderDesc);
	AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not create shader description from reflector!!!", m_hr,
		return false);
	
	if(!InitializeShaderResourceSlots(pReflector))
	{
		AELOG_ERROR(ENGINE_LOGGER, L"Could not initialize shader resource slots!!!");
		return false;
	}

	if(!CreateConstantBuffers(pDevice, vsBlob, pReflector))
	{
		AELOG_ERROR(ENGINE_LOGGER, L"Could not create constant buffers!!!");
		return false;
	}

	return true;
}

//creates shader resource views based on what is found
ABOOL Shader::InitializeShaderResourceSlots(ID3D11ShaderReflection *pReflector)
{
	shared_array<D3D11_SHADER_INPUT_BIND_DESC> pShaderResourceDescs(new
		D3D11_SHADER_INPUT_BIND_DESC[m_shaderDesc.BoundResources]);

	/*********************************************************
		Even though both samplers and textures are considered
		Shader resources during reflection, they do NOT occupy
		the same slots.
	**********************************************************/
	for(unsigned int srvBindIndex = 0; srvBindIndex < m_shaderDesc.BoundResources; ++srvBindIndex)
	{
		m_hr = pReflector->GetResourceBindingDesc(srvBindIndex, &pShaderResourceDescs[srvBindIndex]);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could get resource binding description from reflector!!!", m_hr,
			return false);

		if(pShaderResourceDescs[srvBindIndex].Type == D3D10_SIT_SAMPLER)
		{
			m_SamplerStateByName[pShaderResourceDescs[srvBindIndex].Name] = m_nSamplerStates;
			++m_nSamplerStates;
		}
		else
		{
			m_ShaderResourceViewByName[pShaderResourceDescs[srvBindIndex].Name] = m_nShaderResourceViews;
			++m_nShaderResourceViews;
		}

		/*
		format str = format("SHADER RESOURCE VIEW:  Name: %1% || Type: %2% || BindPoint: %3% || BindCount: %4% || ReturnType: %5% || Dimension: %6%")
			% pShaderResourceDescs[srvBindIndex].Name
			% pShaderResourceDescs[srvBindIndex].Type
			% pShaderResourceDescs[srvBindIndex].BindPoint
			% pShaderResourceDescs[srvBindIndex].BindPoint
			% pShaderResourceDescs[srvBindIndex].ReturnType
			% pShaderResourceDescs[srvBindIndex].Dimension;
		wcout << str.str().c_str() << endl;
		*/
	}

	m_pShaderResourceViews = shared_array<ID3D11ShaderResourceView *>(new ID3D11ShaderResourceView*[m_nShaderResourceViews]);
	for(unsigned int srvIndex = 0; srvIndex < m_nShaderResourceViews; ++srvIndex)
	{
		m_pShaderResourceViews[srvIndex] = nullptr;
	}

	m_pSamplerStates = shared_array<ID3D11SamplerState *>(new ID3D11SamplerState*[m_nSamplerStates]);
	for(unsigned int ssIndex = 0; ssIndex < m_nSamplerStates; ++ssIndex)
	{
		m_pSamplerStates[ssIndex] = nullptr;
	}

	return true;
}

//creates constant buffers using the reflection interface
ABOOL Shader::CreateConstantBuffers(ID3D11Device *pDevice, ID3D10Blob *pBlob, ID3D11ShaderReflection *pReflector)
{
	m_pConstantBuffers = shared_array<ID3D11Buffer*>(
		new ID3D11Buffer*[m_shaderDesc.ConstantBuffers]);
	m_pConstantBufferDescs = shared_array<D3D11_SHADER_BUFFER_DESC>(
		new D3D11_SHADER_BUFFER_DESC[m_shaderDesc.ConstantBuffers]);

	for(unsigned int cbIndex = 0; cbIndex < m_shaderDesc.ConstantBuffers; ++cbIndex)
	{
		ID3D11ShaderReflectionConstantBuffer *srCb =
			pReflector->GetConstantBufferByIndex(cbIndex);
		if(srCb)
		{
			m_hr = srCb->GetDesc(&m_pConstantBufferDescs[cbIndex]);
			AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not get description of shader cbuffer!!!", m_hr, return false);

			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.ByteWidth = m_pConstantBufferDescs[cbIndex].Size;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			/*format str = format("SHADER CBUFFER:  Name: %1% || Size: %2% || Type: %3% || Flags: %4% || Variables %5%")
				% m_pConstantBufferDescs[cbIndex].Name
				% m_pConstantBufferDescs[cbIndex].Size
				% m_pConstantBufferDescs[cbIndex].Type
				% m_pConstantBufferDescs[cbIndex].uFlags
				% m_pConstantBufferDescs[cbIndex].Variables;

			wcout << str.str().c_str() << endl; */
			m_hr = pDevice->CreateBuffer(&bufferDesc, NULL, &m_pConstantBuffers[cbIndex]);
			AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not create shader cbuffer!!!", m_hr, return false);
			//add to stl map to be looked up by name later
			m_mapCBufferByName[m_pConstantBufferDescs[cbIndex].Name] = 
				cbIndex;
		}
		else
		{
			AELOG_ERROR(ENGINE_LOGGER, L"Could not get reflection for constant buffer!!!");
			return false;
		}
	}

	return true;
}

void Shader::SetConstantBufferDataByName(ID3D11DeviceContext *pDeviceContext, const char *name, void const *pData)
{
	SetConstantBufferDataByIndex(pDeviceContext, m_mapCBufferByName.find(name)->second, pData);
}

void Shader::SetConstantBufferDataByIndex(ID3D11DeviceContext *pDeviceContext, unsigned int index, void const *pData)
{
	SetConstantBufferData(pDeviceContext, m_pConstantBuffers[index], pData);
}

//sets a buffer to the data passed in - no validation, caller is expected to know how large the buffer being set should be
void Shader::SetConstantBufferData(ID3D11DeviceContext *pDeviceContext, ID3D11Buffer *pCBuffer, void const *pData)
{
	//will automatically copy over the entire size of structure
	pDeviceContext->UpdateSubresource(pCBuffer, 0, NULL, pData, 0, 0);
}

//applies constant buffers to rendering pipeline
void Shader::ApplyConstantBuffers(ID3D11DeviceContext *pDeviceContext, 
		std::function<void (ID3D11DeviceContext *, UINT, UINT, ID3D11Buffer *const *)> applyConstants)
{
	applyConstants(pDeviceContext, 0, m_shaderDesc.ConstantBuffers, m_pConstantBuffers.get());
}

void Shader::SetShaderResourceByName(ID3D11DeviceContext *pDeviceContext, 
	const char *name, ID3D11ShaderResourceView *pShaderResourceView)
{
	SetShaderResourceByIndex(pDeviceContext, m_ShaderResourceViewByName.find(name)->second,
		pShaderResourceView);
}

void Shader::SetShaderResourceByIndex(ID3D11DeviceContext *pDeviceContext,
	unsigned int index, ID3D11ShaderResourceView *pShaderResourceView)
{
	m_pShaderResourceViews[index] = pShaderResourceView;
}

//applies shader resource to rendering pipeline at whatever stage is specified by function
void Shader::ApplyShaderResources(ID3D11DeviceContext *pDeviceContext, 
	std::function<void (ID3D11DeviceContext *, UINT, UINT, ID3D11ShaderResourceView * const *)> applyShaderResources)
{
	applyShaderResources(pDeviceContext, 0, m_nShaderResourceViews, m_pShaderResourceViews.get());
}


void Shader::SetSamplerStateByName(ID3D11DeviceContext *pDeviceContext, 
	const char *name, ID3D11SamplerState *pSamplerState)
{
	SetSamplerStateByIndex(pDeviceContext, m_SamplerStateByName.find(name)->second, pSamplerState);
}

void Shader::SetSamplerStateByIndex(ID3D11DeviceContext *pDeviceContext, 
	unsigned int index, ID3D11SamplerState *pSamplerState)
{
	m_pSamplerStates[index] = pSamplerState;
}

//applies sampler state to rendering pipeline at whatever stage is specified by function
void Shader::ApplySamplerStates(ID3D11DeviceContext *pDeviceContext,
	std::function<void (ID3D11DeviceContext *, UINT, UINT, ID3D11SamplerState *const *)> applySamplerStates)
{
	applySamplerStates(pDeviceContext, 0, m_nSamplerStates, m_pSamplerStates.get());
}

}