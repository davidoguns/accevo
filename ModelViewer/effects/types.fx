#ifndef _FXTYPES_FX
#define _FXTYPES_FX



//Not all of the following types are used in all shaders

struct PS_OUTPUT
{
	float4 color : SV_TARGET;
};

struct VS_INPUT
{
	float4 pos : SV_POSITION0;
	float4 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
};

#endif