//David Oguns
//GeometryShader.cpp
//December 30, 2012

#include "dxutil.h"
#include "GeometryShader.h"
#include "EngineKernel.h"
#include <boost/format.hpp>

using boost::wformat;

namespace Accevo
{

GeometryShader::GeometryShader(ID3D11Device *pDevice, wchar_t const *filename)
{
	ID3DBlob *gsBlob = nullptr;
	ID3DBlob *errorBlob = nullptr;

	m_hr = D3DCompileFromFile(filename,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,					//no macros, no includes
		"gs_main",							//name of shader entry point function
		"gs_5_0",							//shader target version
		0,									//only used if this is an FX file
		0,									//no thread pump - compile synchronously
		&gsBlob,
		&errorBlob);
	AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not compile geometry shader from file!!!", m_hr,
		if(errorBlob)
		{
			errorBlob->Release();
			errorBlob = nullptr;
		}
		return;
	);

	m_hr = pDevice->CreateGeometryShader(gsBlob->GetBufferPointer(),
		gsBlob->GetBufferSize(),
		NULL,		//no linking
		(ID3D11GeometryShader **)&m_pShader);
	AELOG_DXERR_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, L"Could not create geometry shader from blob!!!", m_hr,
		return;
	);

	ID3D11ShaderReflection *pReflector = nullptr;
	if(!ReflectShader(pDevice, gsBlob, pReflector))
	{
		AELOG_ERROR(ENGINE_LOGGER, L"Could not get info from shader to complete loading!!!");
		return;
	}

	if(gsBlob)
	{
		gsBlob->Release();
		gsBlob = nullptr;
	}

	AELOG_INFO(ENGINE_LOGGER, (wformat(L"Loaded [%1%] successfully!!!")%filename).str().c_str());
	m_isInitialized = true;
}

GeometryShader::~GeometryShader()
{

}

void GeometryShader::AttachShader(ID3D11DeviceContext *pDeviceContext)
{
	Shader::ApplySamplerStates(pDeviceContext, &ID3D11DeviceContext::GSSetSamplers);
	Shader::ApplyShaderResources(pDeviceContext, &ID3D11DeviceContext::GSSetShaderResources);
	Shader::ApplyConstantBuffers(pDeviceContext, &ID3D11DeviceContext::GSSetConstantBuffers);
	pDeviceContext->GSSetShader((ID3D11GeometryShader *)m_pShader, NULL, 0);
}

}