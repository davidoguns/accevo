//David Oguns
//DirectX11PropertyLoader.cpp

#include "DirectX11DataHelper.h"
#include "StringIntern.h"
#include "StringMap.h"
#include <string>
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>

namespace Accevo
{
	using boost::property_tree::ptree;
	using boost::optional;
	
	void GetStringTokenList(char *string, char const *delim, std::list<char *> & tokens)
	{
		char *restoftokens;
		string = strtok_s(string, delim, &restoftokens);
		while(string != nullptr)
		{
			tokens.push_back(string);
			string = strtok_s(nullptr, delim, &restoftokens);
		}
	}

	void SelectPTreeChildren(char const *name, boost::property_tree::ptree const &pt,
		std::list<boost::property_tree::ptree const *>& childrenOut)
	{
		std::for_each(pt.begin(), pt.end(),
			[&] (std::pair<std::string, boost::property_tree::ptree const &> const & child) -> void
			{
				if(child.first.compare(name) == 0)
				{
					childrenOut.push_back(&child.second);
				}
			}
		);
	}

	bool									DirectX11DataHelper::s_bInitialized = false;
	//declare static variables
	StringMap<DXGI_FORMAT>					DirectX11DataHelper::s_dxgiFormats;
	StringMap<D3D11_SRV_DIMENSION>			DirectX11DataHelper::s_srvDimension;
	StringMap<UINT>							DirectX11DataHelper::s_bufferExFlags;
	StringMap<D3D11_INPUT_CLASSIFICATION>	DirectX11DataHelper::s_inputClassifications;
	StringMap<D3D11_USAGE>					DirectX11DataHelper::s_d3dusage;
	StringMap<UINT>							DirectX11DataHelper::s_bindFlags;
	StringMap<UINT>							DirectX11DataHelper::s_cpuAccessFlags;
	StringMap<D3D11_RESOURCE_MISC_FLAG>		DirectX11DataHelper::s_resourceMiscFlags;
	StringMap<UINT>							DirectX11DataHelper::s_d3dxfilterFlags;

	void DirectX11DataHelper::Initialize()
	{
		//don't waste time initializing more than once
		if(!s_bInitialized)
		{
			//populate dxgi format map - this code was generated after
			//parsing the enum from the header file with 
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_UNKNOWN"), DXGI_FORMAT_UNKNOWN);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32B32A32_TYPELESS"), DXGI_FORMAT_R32G32B32A32_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32B32A32_FLOAT"), DXGI_FORMAT_R32G32B32A32_FLOAT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32B32A32_UINT"), DXGI_FORMAT_R32G32B32A32_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32B32A32_SINT"), DXGI_FORMAT_R32G32B32A32_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32B32_TYPELESS"), DXGI_FORMAT_R32G32B32_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32B32_FLOAT"), DXGI_FORMAT_R32G32B32_FLOAT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32B32_UINT"), DXGI_FORMAT_R32G32B32_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32B32_SINT"), DXGI_FORMAT_R32G32B32_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16B16A16_TYPELESS"), DXGI_FORMAT_R16G16B16A16_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16B16A16_FLOAT"), DXGI_FORMAT_R16G16B16A16_FLOAT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16B16A16_UNORM"), DXGI_FORMAT_R16G16B16A16_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16B16A16_UINT"), DXGI_FORMAT_R16G16B16A16_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16B16A16_SNORM"), DXGI_FORMAT_R16G16B16A16_SNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16B16A16_SINT"), DXGI_FORMAT_R16G16B16A16_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32_TYPELESS"), DXGI_FORMAT_R32G32_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32_FLOAT"), DXGI_FORMAT_R32G32_FLOAT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32_UINT"), DXGI_FORMAT_R32G32_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G32_SINT"), DXGI_FORMAT_R32G32_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32G8X24_TYPELESS"), DXGI_FORMAT_R32G8X24_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_D32_FLOAT_S8X24_UINT"), DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS"), DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_X32_TYPELESS_G8X24_UINT"), DXGI_FORMAT_X32_TYPELESS_G8X24_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R10G10B10A2_TYPELESS"), DXGI_FORMAT_R10G10B10A2_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R10G10B10A2_UNORM"), DXGI_FORMAT_R10G10B10A2_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R10G10B10A2_UINT"), DXGI_FORMAT_R10G10B10A2_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R11G11B10_FLOAT"), DXGI_FORMAT_R11G11B10_FLOAT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8B8A8_TYPELESS"), DXGI_FORMAT_R8G8B8A8_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8B8A8_UNORM"), DXGI_FORMAT_R8G8B8A8_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8B8A8_UNORM_SRGB"), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8B8A8_UINT"), DXGI_FORMAT_R8G8B8A8_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8B8A8_SNORM"), DXGI_FORMAT_R8G8B8A8_SNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8B8A8_SINT"), DXGI_FORMAT_R8G8B8A8_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16_TYPELESS"), DXGI_FORMAT_R16G16_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16_FLOAT"), DXGI_FORMAT_R16G16_FLOAT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16_UNORM"), DXGI_FORMAT_R16G16_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16_UINT"), DXGI_FORMAT_R16G16_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16_SNORM"), DXGI_FORMAT_R16G16_SNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16G16_SINT"), DXGI_FORMAT_R16G16_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32_TYPELESS"), DXGI_FORMAT_R32_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_D32_FLOAT"), DXGI_FORMAT_D32_FLOAT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32_FLOAT"), DXGI_FORMAT_R32_FLOAT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32_UINT"), DXGI_FORMAT_R32_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R32_SINT"), DXGI_FORMAT_R32_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R24G8_TYPELESS"), DXGI_FORMAT_R24G8_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_D24_UNORM_S8_UINT"), DXGI_FORMAT_D24_UNORM_S8_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R24_UNORM_X8_TYPELESS"), DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_X24_TYPELESS_G8_UINT"), DXGI_FORMAT_X24_TYPELESS_G8_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8_TYPELESS"), DXGI_FORMAT_R8G8_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8_UNORM"), DXGI_FORMAT_R8G8_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8_UINT"), DXGI_FORMAT_R8G8_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8_SNORM"), DXGI_FORMAT_R8G8_SNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8_SINT"), DXGI_FORMAT_R8G8_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16_TYPELESS"), DXGI_FORMAT_R16_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16_FLOAT"), DXGI_FORMAT_R16_FLOAT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_D16_UNORM"), DXGI_FORMAT_D16_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16_UNORM"), DXGI_FORMAT_R16_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16_UINT"), DXGI_FORMAT_R16_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16_SNORM"), DXGI_FORMAT_R16_SNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R16_SINT"), DXGI_FORMAT_R16_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8_TYPELESS"), DXGI_FORMAT_R8_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8_UNORM"), DXGI_FORMAT_R8_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8_UINT"), DXGI_FORMAT_R8_UINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8_SNORM"), DXGI_FORMAT_R8_SNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8_SINT"), DXGI_FORMAT_R8_SINT);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_A8_UNORM"), DXGI_FORMAT_A8_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R1_UNORM"), DXGI_FORMAT_R1_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R9G9B9E5_SHAREDEXP"), DXGI_FORMAT_R9G9B9E5_SHAREDEXP);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R8G8_B8G8_UNORM"), DXGI_FORMAT_R8G8_B8G8_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_G8R8_G8B8_UNORM"), DXGI_FORMAT_G8R8_G8B8_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC1_TYPELESS"), DXGI_FORMAT_BC1_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC1_UNORM"), DXGI_FORMAT_BC1_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC1_UNORM_SRGB"), DXGI_FORMAT_BC1_UNORM_SRGB);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC2_TYPELESS"), DXGI_FORMAT_BC2_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC2_UNORM"), DXGI_FORMAT_BC2_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC2_UNORM_SRGB"), DXGI_FORMAT_BC2_UNORM_SRGB);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC3_TYPELESS"), DXGI_FORMAT_BC3_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC3_UNORM"), DXGI_FORMAT_BC3_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC3_UNORM_SRGB"), DXGI_FORMAT_BC3_UNORM_SRGB);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC4_TYPELESS"), DXGI_FORMAT_BC4_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC4_UNORM"), DXGI_FORMAT_BC4_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC4_SNORM"), DXGI_FORMAT_BC4_SNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC5_TYPELESS"), DXGI_FORMAT_BC5_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC5_UNORM"), DXGI_FORMAT_BC5_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC5_SNORM"), DXGI_FORMAT_BC5_SNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_B5G6R5_UNORM"), DXGI_FORMAT_B5G6R5_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_B5G5R5A1_UNORM"), DXGI_FORMAT_B5G5R5A1_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_B8G8R8A8_UNORM"), DXGI_FORMAT_B8G8R8A8_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_B8G8R8X8_UNORM"), DXGI_FORMAT_B8G8R8X8_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM"), DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_B8G8R8A8_TYPELESS"), DXGI_FORMAT_B8G8R8A8_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_B8G8R8A8_UNORM_SRGB"), DXGI_FORMAT_B8G8R8A8_UNORM_SRGB);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_B8G8R8X8_TYPELESS"), DXGI_FORMAT_B8G8R8X8_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_B8G8R8X8_UNORM_SRGB"), DXGI_FORMAT_B8G8R8X8_UNORM_SRGB);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC6H_TYPELESS"), DXGI_FORMAT_BC6H_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC6H_UF16"), DXGI_FORMAT_BC6H_UF16);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC6H_SF16"), DXGI_FORMAT_BC6H_SF16);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC7_TYPELESS"), DXGI_FORMAT_BC7_TYPELESS);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC7_UNORM"), DXGI_FORMAT_BC7_UNORM);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_BC7_UNORM_SRGB"), DXGI_FORMAT_BC7_UNORM_SRGB);
			s_dxgiFormats.Insert(SID("DXGI_FORMAT_FORCE_UINT"), DXGI_FORMAT_FORCE_UINT);

			//populate shader resource view dimension map
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_UNKNOWN"), D3D_SRV_DIMENSION_UNKNOWN);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_BUFFER"), D3D_SRV_DIMENSION_BUFFER);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_TEXTURE1D"), D3D_SRV_DIMENSION_TEXTURE1D);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_TEXTURE1DARRAY"), D3D_SRV_DIMENSION_TEXTURE1DARRAY);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_TEXTURE2D"), D3D_SRV_DIMENSION_TEXTURE2D);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_TEXTURE2DARRAY"), D3D_SRV_DIMENSION_TEXTURE2DARRAY);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_TEXTURE2DMS"), D3D_SRV_DIMENSION_TEXTURE2DMS);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_TEXTURE2DMSARRAY"), D3D_SRV_DIMENSION_TEXTURE2DMSARRAY);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_TEXTURE3D"), D3D_SRV_DIMENSION_TEXTURE3D);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_TEXTURECUBE"), D3D_SRV_DIMENSION_TEXTURECUBE);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_TEXTURECUBEARRAY"), D3D_SRV_DIMENSION_TEXTURECUBEARRAY);
			s_srvDimension.Insert(SID("D3D_SRV_DIMENSION_BUFFEREX"), D3D_SRV_DIMENSION_BUFFEREX);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_UNKNOWN"), D3D10_SRV_DIMENSION_UNKNOWN);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_BUFFER"), D3D10_SRV_DIMENSION_BUFFER);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_TEXTURE1D"), D3D10_SRV_DIMENSION_TEXTURE1D);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_TEXTURE1DARRAY"), D3D10_SRV_DIMENSION_TEXTURE1DARRAY);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_TEXTURE2D"), D3D10_SRV_DIMENSION_TEXTURE2D);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_TEXTURE2DARRAY"), D3D10_SRV_DIMENSION_TEXTURE2DARRAY);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_TEXTURE2DMS"), D3D10_SRV_DIMENSION_TEXTURE2DMS);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY"), D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_TEXTURE3D"), D3D10_SRV_DIMENSION_TEXTURE3D);
			s_srvDimension.Insert(SID("D3D10_SRV_DIMENSION_TEXTURECUBE"), D3D10_SRV_DIMENSION_TEXTURECUBE);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_UNKNOWN"), D3D10_1_SRV_DIMENSION_UNKNOWN);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_BUFFER"), D3D10_1_SRV_DIMENSION_BUFFER);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_TEXTURE1D"), D3D10_1_SRV_DIMENSION_TEXTURE1D);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_TEXTURE1DARRAY"), D3D10_1_SRV_DIMENSION_TEXTURE1DARRAY);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_TEXTURE2D"), D3D10_1_SRV_DIMENSION_TEXTURE2D);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY"), D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_TEXTURE2DMS"), D3D10_1_SRV_DIMENSION_TEXTURE2DMS);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_TEXTURE2DMSARRAY"), D3D10_1_SRV_DIMENSION_TEXTURE2DMSARRAY);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_TEXTURE3D"), D3D10_1_SRV_DIMENSION_TEXTURE3D);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_TEXTURECUBE"), D3D10_1_SRV_DIMENSION_TEXTURECUBE);
			s_srvDimension.Insert(SID("D3D10_1_SRV_DIMENSION_TEXTURECUBEARRAY"), D3D10_1_SRV_DIMENSION_TEXTURECUBEARRAY);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_UNKNOWN"), D3D11_SRV_DIMENSION_UNKNOWN);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_BUFFER"), D3D11_SRV_DIMENSION_BUFFER);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_TEXTURE1D"), D3D11_SRV_DIMENSION_TEXTURE1D);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_TEXTURE1DARRAY"), D3D11_SRV_DIMENSION_TEXTURE1DARRAY);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_TEXTURE2D"), D3D11_SRV_DIMENSION_TEXTURE2D);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_TEXTURE2DARRAY"), D3D11_SRV_DIMENSION_TEXTURE2DARRAY);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_TEXTURE2DMS"), D3D11_SRV_DIMENSION_TEXTURE2DMS);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY"), D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_TEXTURE3D"), D3D11_SRV_DIMENSION_TEXTURE3D);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_TEXTURECUBE"), D3D11_SRV_DIMENSION_TEXTURECUBE);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_TEXTURECUBEARRAY"), D3D11_SRV_DIMENSION_TEXTURECUBEARRAY);
			s_srvDimension.Insert(SID("D3D11_SRV_DIMENSION_BUFFEREX"), D3D11_SRV_DIMENSION_BUFFEREX);


			s_bufferExFlags.Insert(SID("D3D11_BUFFEREX_SRV_FLAG_RAW"), D3D11_BUFFEREX_SRV_FLAG_RAW);

			s_inputClassifications.Insert(SID("D3D11_INPUT_PER_VERTEX_DATA"), D3D11_INPUT_PER_VERTEX_DATA);
			s_inputClassifications.Insert(SID("D3D11_INPUT_PER_INSTANCE_DATA"), D3D11_INPUT_PER_INSTANCE_DATA);

			s_d3dusage.Insert(SID("D3D11_USAGE_DEFAULT"), D3D11_USAGE_DEFAULT);
			s_d3dusage.Insert(SID("D3D11_USAGE_IMMUTABLE"), D3D11_USAGE_IMMUTABLE);
			s_d3dusage.Insert(SID("D3D11_USAGE_DYNAMIC"), D3D11_USAGE_DYNAMIC);
			s_d3dusage.Insert(SID("D3D11_USAGE_STAGING"), D3D11_USAGE_STAGING);

			s_bindFlags.Insert(SID("D3D11_BIND_VERTEX_BUFFER"), D3D11_BIND_VERTEX_BUFFER);
			s_bindFlags.Insert(SID("D3D11_BIND_INDEX_BUFFER"), D3D11_BIND_INDEX_BUFFER);
			s_bindFlags.Insert(SID("D3D11_BIND_CONSTANT_BUFFER"), D3D11_BIND_CONSTANT_BUFFER);
			s_bindFlags.Insert(SID("D3D11_BIND_SHADER_RESOURCE"), D3D11_BIND_SHADER_RESOURCE);
			s_bindFlags.Insert(SID("D3D11_BIND_STREAM_OUTPUT"), D3D11_BIND_STREAM_OUTPUT);
			s_bindFlags.Insert(SID("D3D11_BIND_RENDER_TARGET"), D3D11_BIND_RENDER_TARGET);
			s_bindFlags.Insert(SID("D3D11_BIND_DEPTH_STENCIL"), D3D11_BIND_DEPTH_STENCIL);
			s_bindFlags.Insert(SID("D3D11_BIND_UNORDERED_ACCESS"), D3D11_BIND_UNORDERED_ACCESS);


			s_cpuAccessFlags.Insert(SID("D3D11_CPU_ACCESS_WRITE"), D3D11_CPU_ACCESS_WRITE);
			s_cpuAccessFlags.Insert(SID("D3D11_CPU_ACCESS_READ"), D3D11_CPU_ACCESS_READ);

			s_resourceMiscFlags.Insert(SID("D3D11_RESOURCE_MISC_GENERATE_MIPS"), D3D11_RESOURCE_MISC_GENERATE_MIPS);
			s_resourceMiscFlags.Insert(SID("D3D11_RESOURCE_MISC_SHARED"), D3D11_RESOURCE_MISC_SHARED);
			s_resourceMiscFlags.Insert(SID("D3D11_RESOURCE_MISC_TEXTURECUBE"), D3D11_RESOURCE_MISC_TEXTURECUBE);
			s_resourceMiscFlags.Insert(SID("D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS"), D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS);
			s_resourceMiscFlags.Insert(SID("D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS"), D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS);
			s_resourceMiscFlags.Insert(SID("D3D11_RESOURCE_MISC_BUFFER_STRUCTURED"), D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);
			s_resourceMiscFlags.Insert(SID("D3D11_RESOURCE_MISC_RESOURCE_CLAMP"), D3D11_RESOURCE_MISC_RESOURCE_CLAMP);
			s_resourceMiscFlags.Insert(SID("D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX"), D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX);
			s_resourceMiscFlags.Insert(SID("D3D11_RESOURCE_MISC_GDI_COMPATIBLE"), D3D11_RESOURCE_MISC_GDI_COMPATIBLE);

			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_NONE"), D3DX11_FILTER_NONE);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_POINT"), D3DX11_FILTER_POINT);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_LINEAR"), D3DX11_FILTER_LINEAR);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_TRIANGLE"), D3DX11_FILTER_TRIANGLE);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_BOX"), D3DX11_FILTER_BOX);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_MIRROR_U"), D3DX11_FILTER_MIRROR_U);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_MIRROR_V"), D3DX11_FILTER_MIRROR_V);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_MIRROR_W"), D3DX11_FILTER_MIRROR_W);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_MIRROR"), D3DX11_FILTER_MIRROR);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_DITHER"), D3DX11_FILTER_DITHER);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_DITHER_DIFFUSION"), D3DX11_FILTER_DITHER_DIFFUSION);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_SRGB_IN"), D3DX11_FILTER_SRGB_IN);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_SRGB_OUT"), D3DX11_FILTER_SRGB_OUT);
			s_d3dxfilterFlags.Insert(SID("D3DX11_FILTER_SRGB"), D3DX11_FILTER_SRGB);
		}
	}

	template<class T>
	bool DirectX11DataHelper::GetFlags(const char *stringFlags, T &out, StringMap<T> const &map)
	{
		bool fullyPopulated = true;
		char *buffer = new char[strlen(stringFlags)+1];
		std::list<char *> tokens;
		GetStringTokenList(buffer, " \t\r\n", tokens);
		std::for_each(tokens.begin(), tokens.end(),
			[&](char *token)
			{
				boost::optional<T> value = map.GetOptional(HashString(token));
				if(value)
				{
					out |= *value;
				}
				else
				{
					fullyPopulated = false;
				}
			}
		);

		delete [] buffer;
		return fullyPopulated;
	}

	template<class T>
	bool DirectX11DataHelper::GetProperty(StringIdentifier const &sid, T & dataOut, StringMap<T> const &map)
	{
		boost::optional<T> ret = map.GetOptional(sid);
		if(ret)
		{
			dataOut = *ret;
			return true;
		}
		return false;
	}

	bool DirectX11DataHelper::GetDxgiFormat(char const *formatString, DXGI_FORMAT & formatOut)
	{
		return GetDxgiFormat(HashString(formatString), formatOut);
	}

	bool DirectX11DataHelper::GetDxgiFormat(StringIdentifier const &sid, DXGI_FORMAT & formatOut)
	{
		return GetProperty<DXGI_FORMAT>(sid, formatOut, s_dxgiFormats);
	}

	bool DirectX11DataHelper::GetShaderResourceViewDimension(char const *dimensionString, D3D11_SRV_DIMENSION & dimensionOut)
	{
		return GetShaderResourceViewDimension(HashString(dimensionString), dimensionOut);
	}
	bool DirectX11DataHelper::GetShaderResourceViewDimension(StringIdentifier const &sid, D3D11_SRV_DIMENSION & dimensionOut)
	{
		return GetProperty<D3D11_SRV_DIMENSION>(sid, dimensionOut, s_srvDimension);
	}

	bool DirectX11DataHelper::GetD3D11Usage(char const *stringUsage, D3D11_USAGE &usage)
	{
		return GetD3D11Usage(HashString(stringUsage), usage);
	}

	bool DirectX11DataHelper::GetD3D11Usage(StringIdentifier const &sid, D3D11_USAGE &usage)
	{
		return GetProperty<D3D11_USAGE>(sid, usage, s_d3dusage);
	}

	bool DirectX11DataHelper::GetShaderResourceViewDesc(ptree const &pt, D3D11_SHADER_RESOURCE_VIEW_DESC & srvDesc)
	{
		bool fullyPopulated = true;
		optional<std::string> value = pt.get_optional<std::string>("<xmlattr>.format");
		//if value isn't there, or value is there, but didn't resolve to a valid DXGI_FORMAT
		if(!(value && GetDxgiFormat(value->c_str(), srvDesc.Format)))
		{	//then we haven't fully populated this struct
			fullyPopulated = false;
		}

		value = pt.get_optional<std::string>("<xmlattr>.viewDimension");
		if(!(value && GetShaderResourceViewDimension(value->c_str(), srvDesc.ViewDimension)))
		{
			fullyPopulated = false;
		}
		else
		{	//only run this code if a view dimension is found
			optional<ptree const &> childSRV;
			switch(srvDesc.ViewDimension)
			{
			case D3D11_SRV_DIMENSION_BUFFER:
				childSRV = pt.get_child_optional("bufferSRV");
				if(!(childSRV && GetBufferShaderResourceViewDesc(*childSRV, srvDesc.Buffer)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_TEXTURE1D:
				childSRV = pt.get_child_optional("tex1DSRV");
				if(!(childSRV && GetTexture1DShaderResourceViewDesc(*childSRV, srvDesc.Texture1D)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
				childSRV = pt.get_child_optional("tex1DArraySRV");
				if(!(childSRV && GetTexture1DArrayShaderResourceViewDesc(*childSRV, srvDesc.Texture1DArray)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_TEXTURE2D:
				childSRV = pt.get_child_optional("tex2DSRV");
				if(!(childSRV && GetTexture2DShaderResourceViewDesc(*childSRV, srvDesc.Texture2D)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
				childSRV = pt.get_child_optional("tex2DArraySRV");
				if(!(childSRV && GetTexture2DArrayShaderResourceViewDesc(*childSRV, srvDesc.Texture2DArray)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_TEXTURE2DMS:
				childSRV = pt.get_child_optional("tex2DMSSRV");
				if(!(childSRV && GetTexture2DMSShaderResourceViewDesc(*childSRV, srvDesc.Texture2DMS)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
				childSRV = pt.get_child_optional("tex2DMSArray");
				if(!(childSRV && GetTexture2DMSArrayShaderResourceViewDesc(*childSRV, srvDesc.Texture2DMSArray)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_TEXTURE3D:
				childSRV = pt.get_child_optional("tex3DSRV");
				if(!(childSRV && GetTexture3DShaderResourceViewDesc(*childSRV, srvDesc.Texture3D)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_TEXTURECUBE:
				childSRV = pt.get_child_optional("texCubeSRV");
				if(!(childSRV && GetTextureCubeShaderResourceViewDesc(*childSRV, srvDesc.TextureCube)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
				childSRV = pt.get_child_optional("texCubeArraySRV");
				if(!(childSRV && GetTextureCubeArrayShaderResourceViewDesc(*childSRV, srvDesc.TextureCubeArray)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_BUFFEREX:
				childSRV = pt.get_child_optional("bufferExSRV");
				if(!(childSRV && GetBufferExShaderResourceViewDesc(*childSRV, srvDesc.BufferEx)))
				{
					fullyPopulated = false;
				}
				break;
			case D3D11_SRV_DIMENSION_UNKNOWN:
				fullyPopulated = false;
				break;
			default:
				//honestly shouldn't be possible to get here because value
				//shouldn't have returned a valid value otherwise
				fullyPopulated = false;
				break;
			}
		}


		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetBufferShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_BUFFER_SRV & bufferSRVDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;

		value = pt.get_optional<unsigned int>("<xmlattr>.firstElement");
		if(value)
		{
			bufferSRVDesc.FirstElement = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.numElements");
		if(value)
		{
			bufferSRVDesc.NumElements = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetTexture1DShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX1D_SRV & tex1DSRVDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;

		value = pt.get_optional<unsigned int>("<xmlattr>.mostDetailedMip");
		if(value)
		{
			tex1DSRVDesc.MostDetailedMip = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.mipLevels");
		if(value)
		{
			tex1DSRVDesc.MipLevels = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetTexture1DArrayShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX1D_ARRAY_SRV & tex1DArraySRVDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;

		value = pt.get_optional<unsigned int>("<xmlattr>.mostDetailedMip");
		if(value)
		{
			tex1DArraySRVDesc.MostDetailedMip = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.mipLevels");
		if(value)
		{
			tex1DArraySRVDesc.MipLevels= *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.firstArraySlice");
		if(value)
		{
			tex1DArraySRVDesc.FirstArraySlice = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.arraySize");
		if(value)
		{
			tex1DArraySRVDesc.ArraySize = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetTexture2DShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX2D_SRV &tex2DSRVDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;

		value = pt.get_optional<unsigned int>("<xmlattr>.mostDetailedMip");
		if(value)
		{
			tex2DSRVDesc.MostDetailedMip = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.mipLevels");
		if(value)
		{
			tex2DSRVDesc.MipLevels = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetTexture2DArrayShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX2D_ARRAY_SRV & tex2DArraySRVDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;

		value = pt.get_optional<unsigned int>("<xmlattr>.mostDetailedMip");
		if(value)
		{
			tex2DArraySRVDesc.MostDetailedMip = *value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		value = pt.get_optional<unsigned int>("<xmlattr>.mipLevels");
		if(value)
		{
			tex2DArraySRVDesc.MipLevels = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.firstArraySlice");
		if(value)
		{
			tex2DArraySRVDesc.FirstArraySlice = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.arraySlice");
		if(value)
		{
			tex2DArraySRVDesc.ArraySize = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetTexture2DMSShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX2DMS_SRV & tex2DMSSRV)
	{
		//do nothing to this 
		return true;
	}

	bool DirectX11DataHelper::GetTexture2DMSArrayShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX2DMS_ARRAY_SRV & tex2DMSArrayDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;
		
		value = pt.get_optional<unsigned int>("<xmlattr>.firstArraySlice");
		if(value)
		{
			tex2DMSArrayDesc.FirstArraySlice = *value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		value = pt.get_optional<unsigned int>("<xmlattr>.arraySize");
		if(value)
		{
			tex2DMSArrayDesc.ArraySize = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetTexture3DShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEX3D_SRV & tex3DSRVDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;

		value = pt.get_optional<unsigned int>("<xmlattr>.mostDetailedMip");
		if(value)
		{
			tex3DSRVDesc.MostDetailedMip = *value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		value = pt.get_optional<unsigned int>("<xmlattr>.mipLevels");
		if(value)
		{
			tex3DSRVDesc.MipLevels = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetTextureCubeShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEXCUBE_SRV & texCubeSRVDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;
		
		value = pt.get_optional<unsigned int>("<xmlattr>.mostDetailedMip");
		if(value)
		{
			texCubeSRVDesc.MostDetailedMip = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.mipLevels");
		if(value)
		{
			texCubeSRVDesc.MipLevels = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetTextureCubeArrayShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_TEXCUBE_ARRAY_SRV & texCubeArraySRVDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;
		
		value = pt.get_optional<unsigned int>("<xmlattr>.mostDetailedMip");
		if(value)
		{
			texCubeArraySRVDesc.MostDetailedMip = *value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		value = pt.get_optional<unsigned int>("<xmlattr>.mipLevels");
		if(value)
		{
			texCubeArraySRVDesc.MipLevels = *value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		value = pt.get_optional<unsigned int>("<xmlattr>.first2DArrayFace");
		if(value)
		{
			texCubeArraySRVDesc.First2DArrayFace = *value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		value = pt.get_optional<unsigned int>("<xmlattr>.numCubes");
		if(value)
		{
			texCubeArraySRVDesc.NumCubes = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetBufferExShaderResourceViewDesc(boost::property_tree::ptree const &pt, D3D11_BUFFEREX_SRV & bufferExSRVDesc)
	{
		bool fullyPopulated = true;
		optional<unsigned int> value;
		
		value = pt.get_optional<unsigned int>("<xmlattr>.firstElement");
		if(value)
		{
			bufferExSRVDesc.FirstElement = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		value = pt.get_optional<unsigned int>("<xmlattr>.numElements");
		if(value)
		{
			bufferExSRVDesc.NumElements = *value;
		}
		else
		{
			fullyPopulated = false;
		}

		if(!GetFlags("<xmlattr>.flags", bufferExSRVDesc.Flags, s_bufferExFlags))
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetInputLayoutElementDesc(boost::property_tree::ptree const &pt, D3D11_INPUT_ELEMENT_DESC &ieDesc)
	{
		bool fullyPopulated = true;

		boost::optional<std::string> stringvalue = pt.get_optional<std::string>("<xmlattr>.semanticName");
		if(stringvalue)
		{
			ieDesc.SemanticName = InternString(stringvalue->c_str());
		}
		else
		{
			fullyPopulated = false;
		}

		boost::optional<unsigned int> uintvalue = pt.get_optional<unsigned int>("<xmlattr>.semanticIndex");
		if(uintvalue)
		{
			ieDesc.SemanticIndex = *uintvalue;
		}
		else
		{
			fullyPopulated = false;
		}
		
		uintvalue = pt.get_optional<unsigned int>("<xmlattr>.inputSlot");
		if(uintvalue)
		{
			ieDesc.InputSlot = *uintvalue;
		}
		else
		{
			fullyPopulated = false;
		}
		
		uintvalue = pt.get_optional<unsigned int>("<xmlattr>.alignedByteOffset");
		if(uintvalue)
		{
			ieDesc.AlignedByteOffset = *uintvalue;
		}
		else
		{
			fullyPopulated = false;
		}
		
		uintvalue = pt.get_optional<unsigned int>("<xmlattr>.instanceStepRate");
		if(uintvalue)
		{
			ieDesc.InstanceDataStepRate = *uintvalue;
		}
		else
		{
			fullyPopulated = false;
		}

		stringvalue = pt.get_optional<std::string>("<xmlattr>.format");
		if(!(stringvalue && GetDxgiFormat(stringvalue->c_str(), ieDesc.Format)))
		{
			fullyPopulated = false;
		}

		stringvalue = pt.get_optional<std::string>("<xmlattr>.inputSlotClass");
		if(!(stringvalue && GetInputSlotClassification(stringvalue->c_str(), ieDesc.InputSlotClass)))
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetInputSlotClassification(char const *classString, D3D11_INPUT_CLASSIFICATION & inputClass)
	{
		return GetInputSlotClassification(HashString(classString), inputClass);
	}

	bool DirectX11DataHelper::GetInputSlotClassification(StringIdentifier const &sid, D3D11_INPUT_CLASSIFICATION & inputClass)
	{
		boost::optional<D3D11_INPUT_CLASSIFICATION> value = s_inputClassifications.GetOptional(sid);
		if(value)
		{
			inputClass = *value;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DirectX11DataHelper::GetInputLayoutDesc(boost::property_tree::ptree const &pt, D3D11_INPUT_ELEMENT_DESC *elements, unsigned int numElements)
	{
		bool fullyPopulated = true;
		std::list<boost::property_tree::ptree const *> childPts;
		SelectPTreeChildren("element", pt, childPts);
		std::list<boost::property_tree::ptree const *>::iterator itr = childPts.begin();
		unsigned int elementIndex = 0;
		for(; elementIndex < numElements && itr != childPts.end(); ++elementIndex)
		{
			if(!GetInputLayoutElementDesc((**itr), elements[elementIndex]))
			{	//if one element slips, consider this element not fully ready
				fullyPopulated = false;
			}
			++itr;
		}

		return fullyPopulated;
	}

	bool DirectX11DataHelper::GetD3D11BindFlags(char const *flagsString, UINT &flagsOut)
	{
		return GetFlags<UINT>(flagsString, flagsOut, s_bindFlags);
	}

	bool DirectX11DataHelper::GetD3D11CpuAccessFlags(char const *stringFlags, UINT &flagsOut)
	{
		return GetFlags<UINT>(stringFlags, flagsOut, s_cpuAccessFlags);
	}

	bool DirectX11DataHelper::GetD3DX11FilterFlags(char const *stringFlags, UINT &flagsOut)
	{
		return GetFlags<UINT>(stringFlags, flagsOut, s_d3dxfilterFlags);
	}

	bool DirectX11DataHelper::GetD3DXImageLoadInfo(boost::property_tree::ptree const &pt, D3DX11_IMAGE_LOAD_INFO &imgInfo)
	{
		bool fullyPopulated = true;

		boost::optional<unsigned int> uint_value;
		boost::optional<std::string> string_value;

		uint_value = pt.get_optional<unsigned int>("<xmlattr>.width");
		if(uint_value)
		{
			imgInfo.Width = *uint_value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		uint_value = pt.get_optional<unsigned int>("<xmlattr>.height");
		if(uint_value)
		{
			imgInfo.Height = *uint_value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		uint_value = pt.get_optional<unsigned int>("<xmlattr>.depth");
		if(uint_value)
		{
			imgInfo.Depth = *uint_value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		uint_value = pt.get_optional<unsigned int>("<xmlattr>.firstMipLevel");
		if(uint_value)
		{
			imgInfo.FirstMipLevel = *uint_value;
		}
		else
		{
			fullyPopulated = false;
		}
		
		uint_value = pt.get_optional<unsigned int>("<xmlattr>.mipLevels");
		if(uint_value)
		{
			imgInfo.MipLevels = *uint_value;
		}
		else
		{
			fullyPopulated = false;
		}

		string_value = pt.get_optional<std::string>("<xmlattr>.usage");
		if(!(string_value && GetD3D11Usage(string_value->c_str(), imgInfo.Usage)))
		{
			fullyPopulated = false;
		}
		
		string_value = pt.get_optional<std::string>("<xmlattr>.bindFlags");
		if(!(string_value && GetD3D11BindFlags(string_value->c_str(), imgInfo.BindFlags)))
		{
			fullyPopulated = false;
		}

		return fullyPopulated;
	}

}

