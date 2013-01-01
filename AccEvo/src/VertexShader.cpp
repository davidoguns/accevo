//David Oguns
//December 30, 2012
//VertexShader.cpp

#include "DXUtil.h"
#include "VertexShader.h"
#include "EngineKernel.h"
#include <D3D10.h>
#include <boost/shared_array.hpp>
#include <boost/format.hpp>

namespace Accevo
{
using boost::wformat;
using boost::shared_array;

VertexShader::VertexShader(ID3D11Device *pDevice, wchar_t const *filename) :
	m_pInputLayout(nullptr)
{
	ID3D10Blob *vsBlob = nullptr;
	ID3D10Blob *errorBlob = nullptr;

	m_hr = D3DX11CompileFromFile(filename,
		NULL, NULL,							//no macros, no includes
		"vs_main",							//name of shader entry point function
		"vs_5_0",							//shader target version
		D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG,		//disables old syntax
		NULL,								//only used if this is an FX file
		NULL,								//no thread pump - compile synchronously
		&vsBlob,
		&errorBlob,
		NULL);								//only needed if asynchronously called
	AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not compile vertex shader from file!!!", m_hr,
		if(errorBlob)
		{
			errorBlob->Release();
			errorBlob = nullptr;
		}
		return;
	);

	m_hr = pDevice->CreateVertexShader(vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,		//no linking
		(ID3D11VertexShader**)&m_pShader);
	AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not create vertex shader from blob!!!", m_hr,
		return;
	);

	ID3D11ShaderReflection *pReflector = nullptr;
	if(!ReflectShader(pDevice, vsBlob, pReflector))
	{
		AELOG_ERROR(ENGINE_LOGGER, L"Could not get info from shader to complete loading!!!");
		return;
	}

	if(!CreateInputLayout(pDevice, vsBlob, pReflector))
	{
		AELOG_ERROR(ENGINE_LOGGER, L"Could not create input layout for vertex shader!!!");
	}

	if(vsBlob)
	{
		vsBlob->Release();
		vsBlob = nullptr;
	}

	AELOG_INFO(ENGINE_LOGGER, (wformat(L"Loaded [%1%] successfully!!!") % filename).str().c_str());
	m_isInitialized = true;
}

VertexShader::~VertexShader()
{
	DX_RELEASE(m_pInputLayout);
}

bool VertexShader::CreateInputLayout(ID3D11Device *pDevice, ID3D10Blob *vsBlob, ID3D11ShaderReflection *pReflector)
{
	shared_array<D3D11_SIGNATURE_PARAMETER_DESC> vSPDesc(
		new D3D11_SIGNATURE_PARAMETER_DESC[m_shaderDesc.InputParameters]);
	shared_array<D3D11_INPUT_ELEMENT_DESC> vIEDesc(
		new D3D11_INPUT_ELEMENT_DESC[m_shaderDesc.InputParameters]);

	for(unsigned int inputParamIndex = 0;
		inputParamIndex < m_shaderDesc.InputParameters;
		++inputParamIndex)
	{
		m_hr = pReflector->GetInputParameterDesc(inputParamIndex, &vSPDesc[inputParamIndex]);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not get input parameter description!!!", m_hr, return false);

		//AlignedByteOffset
		vIEDesc[inputParamIndex].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		//Set register type
		switch(vSPDesc[inputParamIndex].ComponentType)
		{
		case D3D10_REGISTER_COMPONENT_UINT32:
			vIEDesc[inputParamIndex].Format =  DXGI_FORMAT_R32G32B32A32_UINT;
			break;
		case D3D10_REGISTER_COMPONENT_SINT32:
			vIEDesc[inputParamIndex].Format = DXGI_FORMAT_R32G32B32A32_SINT;
			break;
		case D3D10_REGISTER_COMPONENT_FLOAT32:
			vIEDesc[inputParamIndex].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case D3D10_REGISTER_COMPONENT_UNKNOWN:
			AELOG_WARN(ENGINE_LOGGER, "Format of register component in shader is unknown!!!");
			return false;
		};
		vIEDesc[inputParamIndex].InputSlot = 0;
		vIEDesc[inputParamIndex].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vIEDesc[inputParamIndex].InstanceDataStepRate = 0;
		vIEDesc[inputParamIndex].SemanticIndex = vSPDesc[inputParamIndex].SemanticIndex;
		vIEDesc[inputParamIndex].SemanticName = vSPDesc[inputParamIndex].SemanticName;

		D3D11_INPUT_ELEMENT_DESC *pElement = &vIEDesc[inputParamIndex];

		//log conditionally
		AELOG_INFO(ENGINE_LOGGER, (wformat(L"INPUT LAYOUT: SemanticName: %1% || SemanticIndex: %2% || SystemValueType: %3% || ComponentType: %4% || Register: %5%")
			% vSPDesc[inputParamIndex].SemanticName
			% vSPDesc[inputParamIndex].SemanticIndex
			% vSPDesc[inputParamIndex].SystemValueType
			% vSPDesc[inputParamIndex].ComponentType
			% vSPDesc[inputParamIndex].Register).str().c_str());
	}

	m_hr = pDevice->CreateInputLayout(vIEDesc.get(), m_shaderDesc.InputParameters,
		vsBlob->GetBufferPointer(),	vsBlob->GetBufferSize(), &m_pInputLayout);
	AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not get create input layout for vertex shader!!!", m_hr, return false);

	return true;
}

void VertexShader::AttachShader(ID3D11DeviceContext *pDeviceContext)
{
	Shader::ApplyConstantBuffers(pDeviceContext, &ID3D11DeviceContext::VSSetConstantBuffers);
	Shader::ApplyShaderResources(pDeviceContext, &ID3D11DeviceContext::VSSetShaderResources);
	Shader::ApplySamplerStates(pDeviceContext, &ID3D11DeviceContext::VSSetSamplers);
	pDeviceContext->IASetInputLayout(m_pInputLayout);
	pDeviceContext->VSSetShader((ID3D11VertexShader*)m_pShader, NULL, 0);
}

}