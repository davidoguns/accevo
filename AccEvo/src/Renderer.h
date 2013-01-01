//David Oguns
//February 19, 2011
//Renderer.h

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <list>
#include <d3dx11effect.h>
#include "HandleManager.h"
#include "StringIdentifier.h"
#include "Logger.h"

namespace Accevo
{
	

	class Renderer
	{
	protected:
		typedef	std::list<ID3DX11EffectPass *>				FXPASSLIST;
		struct MultipassTechnique
		{
			FXPASSLIST						passes;
		};
		typedef HandleManager<MultipassTechnique*>	MULTIPASSTECHNIQUEMANAGER;

	public:
		Renderer(Logger *pLogger, ID3DX11EffectGroup *pEffectGroup);
		~Renderer();

		//Render using a given D3DX Effect Technique using a given device context
		virtual void Render(Handle const &hTechnique, ID3D11DeviceContext *pContext);
		virtual void Render(ID3DX11EffectTechnique *pTechnique, ID3D11DeviceContext *pContext);

	protected:
		ID3DX11EffectGroup						*m_pEffectGroup;
		MultipassTechnique						*m_pTechniques;
		D3DX11_GROUP_DESC						m_groupDesc;
		MULTIPASSTECHNIQUEMANAGER				m_techniqueByName;
		Logger									*m_pLogger;
	};
}

#endif