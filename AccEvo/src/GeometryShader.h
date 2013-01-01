//David Oguns
//GeometryShader.cpp
//December 30, 2012

#pragma once

#include "Shader.h"

namespace Accevo
{

class GeometryShader : public Shader
{
public:
	GeometryShader(ID3D11Device *pDevice, wchar_t const *filename);
	virtual ~GeometryShader();

	virtual void AttachShader(ID3D11DeviceContext *pDeviceContext);
protected:
};

}