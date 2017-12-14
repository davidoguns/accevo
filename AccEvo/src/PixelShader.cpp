//David Oguns
//Pixel Shader

#include "PixelShader.h"
#include "EngineKernel.h"
#include "DXUtil.h"

namespace Accevo
{

PixelShader::PixelShader(ID3D11Device *pDevice, wchar_t const *filename)
{
	ID3DBlob *psBlob = nullptr;
	ID3DBlob *errorBlob = nullptr;
	m_hr = D3DCompileFromFile(filename,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,							//no macros, no includes
		"ps_main",							//name of shader entry point function
		"ps_5_0",							//shader target version
		0,								//only used if this is an FX file
		0,								//no thread pump - compile synchronously
		&psBlob,
		&errorBlob);
	AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not compile pixel shader from file!!!", m_hr,
		if(errorBlob) 
		{
			errorBlob->Release();
			errorBlob = nullptr;
		}
		return;
	);

	m_hr = pDevice->CreatePixelShader(psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,		//no linking
		(ID3D11PixelShader **)&m_pShader);
	AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not create pixel shader from blob!!!", m_hr,
		return;
	);

	ID3D11ShaderReflection *pReflector = nullptr;
	if(!ReflectShader(pDevice, psBlob, pReflector))
	{
		AELOG_ERROR(ENGINE_LOGGER, L"Could not get info from shader to complete loading!!!");
		return;
	}

	if(psBlob)
	{
		psBlob->Release();
		psBlob = nullptr;
	}
	AELOG_INFO(ENGINE_LOGGER, (boost::wformat(L"Loaded [%1%] successfully!!") % filename).str().c_str());
	m_isInitialized = true;
}

PixelShader::~PixelShader()
{

}

void PixelShader::AttachShader(ID3D11DeviceContext *pDeviceContext)
{
	Shader::ApplySamplerStates(pDeviceContext, &ID3D11DeviceContext::PSSetSamplers);
	Shader::ApplyShaderResources(pDeviceContext, &ID3D11DeviceContext::PSSetShaderResources);
	Shader::ApplyConstantBuffers(pDeviceContext, &ID3D11DeviceContext::PSSetConstantBuffers);
	pDeviceContext->PSSetShader((ID3D11PixelShader *)m_pShader, NULL, 0);
}

}