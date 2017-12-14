//David Oguns
//DirectX11PropertyLoader.h


#ifndef _DX11_DATA_HELPER_H_
#define _DX11_DATA_HELPER_H_

#include <D3D11.h>
#include <list>
#include <boost/property_tree/ptree.hpp>
#include "StringMap.h"

namespace Accevo
{
	void GetStringTokenList(char *string, char const *delim, std::list<char *> & tokens);

	void SelectPTreeChildren(char const *name, boost::property_tree::ptree const &pt,
		std::list<boost::property_tree::ptree const *> & childrenOut);

	class DirectX11DataHelper
	{
	public:
		static void Initialize();

		static bool GetDxgiFormat(char const *formatString, DXGI_FORMAT & formatOut);
		static bool GetDxgiFormat(StringIdentifier const &sid, DXGI_FORMAT & formatOut);

		static bool GetShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_SHADER_RESOURCE_VIEW_DESC & srvDesc);
		static bool GetShaderResourceViewDimension(char const *dimensionString, D3D11_SRV_DIMENSION & dimensionOut);
		static bool GetShaderResourceViewDimension(StringIdentifier const &sid, D3D11_SRV_DIMENSION & dimensionOut);
		
		static bool GetInputSlotClassification(StringIdentifier const &sid, D3D11_INPUT_CLASSIFICATION & inputClass);
		static bool GetInputSlotClassification(char const *classString, D3D11_INPUT_CLASSIFICATION & inputClass);

		static bool GetBufferShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_BUFFER_SRV & bufferSRVDesc);
		static bool GetTexture1DShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX1D_SRV & tex1DSRVDesc);
		static bool GetTexture1DArrayShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX1D_ARRAY_SRV & tex1DArraySRVDesc);
		static bool GetTexture2DShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX2D_SRV & tex2DSRVDesc);
		static bool GetTexture2DArrayShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX2D_ARRAY_SRV & tex2DArraySRVDesc);
		static bool GetTexture2DMSShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX2DMS_SRV & tex2DMSSRV);
		static bool GetTexture2DMSArrayShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX2DMS_ARRAY_SRV & tex2DMSArrayDesc);
		static bool GetTexture3DShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX3D_SRV & tex3DSRVDesc);
		static bool GetTextureCubeShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEXCUBE_SRV & texCubeSRVDesc);
		static bool GetTextureCubeArrayShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEXCUBE_ARRAY_SRV & texCubeArraySRVDesc);
		static bool GetBufferExShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_BUFFEREX_SRV & bufferExSRVDesc);

		static bool GetInputLayoutDesc(boost::property_tree::ptree const &pt, D3D11_INPUT_ELEMENT_DESC *elements, unsigned int numElements);
		static bool GetInputLayoutElementDesc(boost::property_tree::ptree const &pt, D3D11_INPUT_ELEMENT_DESC &ieDesc);
		
		//static bool GetD3DXImageLoadInfo(boost::property_tree::ptree const &pt, D3DX11_IMAGE_LOAD_INFO &imgInfo);
		static bool GetD3D11Usage(char const *stringUsage, D3D11_USAGE &usage);
		static bool GetD3D11Usage(StringIdentifier const &sid, D3D11_USAGE &usage);
		static bool GetD3D11BindFlags(char const *flagsString, UINT &flagsOut);
		static bool GetD3D11CpuAccessFlags(char const *stringFlags, UINT &flagsOut);
		//static bool GetD3DX11FilterFlags(char const *stringFlags, UINT &flagsOut);

	protected:
		template<class T>
		static bool GetFlags(const char *stringFlags, T &out, StringMap<T> const &map);

		//templated para
		template<class T>
		static bool GetProperty(StringIdentifier const &sid, T & dataOut, StringMap<T> const &map);

		static bool										s_bInitialized;
		static StringMap<DXGI_FORMAT>					s_dxgiFormats;
		static StringMap<D3D11_SRV_DIMENSION>			s_srvDimension;
		static StringMap<UINT>							s_bufferExFlags;
		static StringMap<D3D11_INPUT_CLASSIFICATION>	s_inputClassifications;
		static StringMap<D3D11_USAGE>					s_d3dusage;
		static StringMap<UINT>							s_bindFlags;
		static StringMap<UINT>							s_cpuAccessFlags;
		static StringMap<D3D11_RESOURCE_MISC_FLAG>		s_resourceMiscFlags;
		static StringMap<UINT>							s_d3dxfilterFlags;
	};
}


#endif