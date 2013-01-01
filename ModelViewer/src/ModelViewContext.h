//David Oguns
//February 17, 2011

#ifndef _MODELVIEWCTX_H_
#define _MODELVIEWCTX_H_

#include <accevo\EngineKernel.h>
#include <accevo\KernelContext.h>
#include <accevo\GraphicsLayer.h>
#include <accevo\DirectInput.h>
#include <accevo\Mesh.h>

#include <accevo\PixelShader.h>
#include <accevo\VertexShader.h>
#include <accevo\GeometryShader.h>

class ModelViewContext : public Accevo::KernelContext
{
public:
	ModelViewContext(Accevo::Logger *pLogger);


	//called when context is about to be run
	virtual void Start();
	//called when context is about to not be run
	virtual void Stop();

	//called before kernel processes are 
	virtual void PreUpdate(float dt);
	virtual void Update(float dt);
	virtual void PostUpdate(float dt);

protected:
	Accevo::Logger						*m_pLogger;
	Accevo::EngineKernel				*m_pKernel;
	Accevo::GraphicsLayer				*m_pGraphics;
	Accevo::DirectInput					*m_pInput;

	//could use more descriptive names eh?
	Accevo::VertexShader				*m_pVS;
	Accevo::PixelShader					*m_pPS;

	Accevo::Mesh						*m_pMesh;
};

#endif