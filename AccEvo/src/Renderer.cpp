//David Oguns
//February 19, 2011

#include "Renderer.h"
#include "DXUtil.h"

namespace Accevo
{
	Renderer::Renderer(Logger *pLogger, ID3DX11EffectGroup *pEffectGroup) :
		m_pLogger(pLogger),
		m_techniqueByName(m_pLogger),
		m_pEffectGroup(pEffectGroup),
		m_pTechniques(nullptr)
	{
		HRESULT hr;
		hr = m_pEffectGroup->GetDesc(&m_groupDesc);
		AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get description of effect group!!", hr, return);
		m_pTechniques = new MultipassTechnique[m_groupDesc.Techniques];
		for(unsigned int techniqueIndex = 0; 
			techniqueIndex <  m_groupDesc.Techniques;
			++techniqueIndex)
		{	//add all techniques to handle manager
			D3DX11_TECHNIQUE_DESC tDesc;
			ID3DX11EffectTechnique *pTechnique = m_pEffectGroup->GetTechniqueByIndex(techniqueIndex);
			pTechnique->GetDesc(&tDesc);
			hr = pTechnique->GetDesc(&tDesc);
			AELOG_DXERR_CONDITIONAL_CODE_ERROR(m_pLogger, L"Could not get description of technique!!", hr, return);
			for(unsigned int passIndex = 0; passIndex < tDesc.Passes; ++passIndex)
			{
				m_pTechniques[techniqueIndex].passes.push_back(pTechnique->GetPassByIndex(passIndex));
			}		
		}
	}

	Renderer::~Renderer()
	{
		if(m_pTechniques)
		{
			delete [] m_pTechniques;
		}
	}

	void Renderer::Render(Handle const &hTechnique, ID3D11DeviceContext *pContext)
	{
		MultipassTechnique *tEntry = m_techniqueByName.GetData(hTechnique);
		std::for_each(tEntry->passes.begin(), tEntry->passes.end(),
			[&] (ID3DX11EffectPass *pEffectPass)
			{
				pEffectPass->Apply(0, pContext);
				//call draw or something here
			}
		);
		//if context is deferred, call finish context?
	}
}