//David Oguns
//December 30, 2012

#pragma once

#include "Shader.h"

namespace Accevo
{

class PixelShader : public Shader
{
public:
	PixelShader(ID3D11Device *pDevice, wchar_t const *filename);
	virtual ~PixelShader();

	virtual void AttachShader(ID3D11DeviceContext *pDeviceContext);
protected:
};

}