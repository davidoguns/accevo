//David Oguns
//December 30, 2012

#pragma once


#include "Shader.h"

namespace Accevo
{

class VertexShader : public Shader
{
public:
	VertexShader(ID3D11Device *pDevice, wchar_t const *filename);
	virtual ~VertexShader();

	virtual void AttachShader(ID3D11DeviceContext *pDeviceContext);

protected:
	bool CreateInputLayout(ID3D11Device *pDevice, ID3D10Blob *vsBlob, ID3D11ShaderReflection *pReflector);
	
	ID3D11InputLayout					*m_pInputLayout;		//input layout of vertex shader
};

}