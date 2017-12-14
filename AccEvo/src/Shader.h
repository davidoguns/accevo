//David Oguns
//Shader.h

#pragma once

#include "aetypes.h"
#include "Logger.h"

#include <D3D11.h>
#include <D3Dcompiler.h>
#include "DXUtil.h"

#include <map>
#include <vector>

#define BOOST_MEM_FN_ENABLE_STDCALL

#include <boost/shared_array.hpp>
#include <boost/optional/optional.hpp>

#include <functional>

/*****************************************************
	This is a mix-in class that allows specializations to
	be written quickly by taking care of the common core
	shader functional responsibilities.  i.e. Creating
	interfaces for setting constant buffers, samplers,
	and shader resources.
******************************************************/
namespace Accevo
{

class Shader
{
public:
	Shader();
	virtual ~Shader();

	Shader(const Shader &) = delete;
	Shader& operator=(const Shader &) = delete;

	ABOOL IsInitialized() const { return m_isInitialized; }

	void SetConstantBufferDataByName(ID3D11DeviceContext *pDeviceContext, const char *name, void const *pData);
	void SetConstantBufferDataByIndex(ID3D11DeviceContext *pDeviceContext, unsigned int index, void const *pData);
	
	//ID3D11ShaderResourceView * GetShaderResourceViewByName(const char *name) const;
	//ID3D11ShaderResourceView * GetShaderResourceView(unsigned int index) const;
	void SetShaderResourceByName(ID3D11DeviceContext *pDeviceContext, 
		const char *name, ID3D11ShaderResourceView *pShaderResourceView);
	void SetShaderResourceByIndex(ID3D11DeviceContext *pDeviceContext,
		unsigned int index, ID3D11ShaderResourceView *pShaderResourceView);

	void SetSamplerStateByName(ID3D11DeviceContext *pDeviceContext, 
		const char *name, ID3D11SamplerState *pSamplerState);
	void SetSamplerStateByIndex(ID3D11DeviceContext *pDeviceContext, 
		unsigned int index, ID3D11SamplerState *pSamplerState);

	//attach shader to rendering pipline.  only concrete shaders know how to do this
	virtual void AttachShader(ID3D11DeviceContext *pDeviceContext) = 0;
	//applies constant buffers to rendering pipeline at whatever stage is specified by function
	void ApplyConstantBuffers(ID3D11DeviceContext *pDeviceContext, 
		 std::function<void (ID3D11DeviceContext *, UINT, UINT, ID3D11Buffer *const *)> applyConstants);
	//applies shader resource to rendering pipeline at whatever stage is specified by function
	void ApplyShaderResources(ID3D11DeviceContext *pDeviceContext, 
		std::function<void (ID3D11DeviceContext *, UINT, UINT, ID3D11ShaderResourceView * const *)> applyShaderResources);
	//applies sampler state to rendering pipeline at whatever stage is specified by function
	void ApplySamplerStates(ID3D11DeviceContext *pDeviceContext,
		std::function<void (ID3D11DeviceContext *, UINT, UINT, ID3D11SamplerState *const *)> applySamplerStates);

	struct ShaderResourceSlot
	{
		ID3D11ShaderResourceView		*pRSV;		//resource view interface
		unsigned int					nSlots;		//number of slots used by this resource view
	};

protected:
	//derives information from the shader loaded into the blob to initialize shader interface fully
	//returns the shader reflector to the caller for derived classes to use further
	ABOOL ReflectShader(ID3D11Device *pDevice, ID3D10Blob *vsBlob, ID3D11ShaderReflection* &pReflector);
	//creates constant buffers using the reflection interface
	ABOOL CreateConstantBuffers(ID3D11Device *pDevice, ID3D10Blob *vsBlob, ID3D11ShaderReflection *pReflector);
	//sets a buffer to the data passed in - no validation, caller is expected to know how large the buffer being set should be
	void SetConstantBufferData(ID3D11DeviceContext *pDevice, ID3D11Buffer *pCBuffer, void const *pData);
	//creates shader resource views based on what is found
	ABOOL InitializeShaderResourceSlots(ID3D11ShaderReflection *pReflector);

	ABOOL								m_isInitialized;			//flag indicating if shader has loaded successfully
	HRESULT								m_hr;						//reused to store result of DX calls
	ID3D11DeviceChild					*m_pShader;					//shader interface
	D3D11_SHADER_DESC					m_shaderDesc;				//shader description

	boost::shared_array<ID3D11Buffer*>					m_pConstantBuffers;		//array of constant buffers to use this vertex shader
	boost::shared_array<D3D11_SHADER_BUFFER_DESC>		m_pConstantBufferDescs;	//array of descriptions to constant buffers
	std::map<const char *, unsigned int, lessThanStr>	m_mapCBufferByName;		//map of names to constant buffer indices

	unsigned int										m_nShaderResourceViews;	//number of resource views
	boost::shared_array<ID3D11ShaderResourceView *>		m_pShaderResourceViews;	//array of resource views
	std::map<const char *, unsigned int, lessThanStr>	m_ShaderResourceViewByName;	//map of names to shader resource indices
	
	unsigned int										m_nSamplerStates;		//number of sampler states
	boost::shared_array<ID3D11SamplerState *>			m_pSamplerStates;		//array of sampler states
	std::map<const char *, unsigned int, lessThanStr>	m_SamplerStateByName;	//map of names to sampler state indices
};

}