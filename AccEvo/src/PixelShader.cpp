//David Oguns
//Pixel Shader

#include "dxutil.h"
#include "PixelShader.h"
#include "EngineKernel.h"
#include "DXUtil.h"
#include <D3DX11.h>

namespace Accevo
{

PixelShader::PixelShader(ID3D11Device *pDevice, wchar_t const *filename)
{
	ID3D10Blob *psBlob = nullptr;
	ID3D10Blob *errorBlob = nullptr;

	m_hr = D3DX11CompileFromFile(filename,
		NULL, NULL,							//no macros, no includes
		"ps_main",							//name of shader entry point function
		"ps_5_0",							//shader target version
		D3D10_SHADER_ENABLE_STRICTNESS,		//disables old syntax
		NULL,								//only used if this is an FX file
		NULL,								//no thread pump - compile synchronously
		&psBlob,
		&errorBlob,
		NULL);								//only needed if asynchronously called
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