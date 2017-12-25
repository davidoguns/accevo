//David Oguns

#include "types.fx"

//vertex shader that does basic transformation
cbuffer cbWorldViewProj
{
	matrix 	world;	
	matrix 	view;
	matrix 	proj;
};

VS_OUTPUT vs_main(in VS_INPUT vs_in)
{
	VS_OUTPUT vs_out;
	matrix tworld = world;	//will be used more than once, calculate ahead
	float4 wpos = mul(vs_in.pos, tworld);
	
	vs_out.pos = mul(mul(mul(vs_in.pos,
		tworld),
		(view)),
		(proj));

	//assuming transformation matrices are affine,
	//we can multiply a normal vector by the matrix as well
	vs_out.normal = mul(vs_in.normal, tworld);
	
	return vs_out;
}
