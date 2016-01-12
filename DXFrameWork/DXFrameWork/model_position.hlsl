//--------------------------------------------------------------------------------------
// Render cube surface position to texture
//
// DirectX/HLSL implementation by
// Christian B. Mendl, 2012
//
// Reference:
// http://graphicsrunner.blogspot.de/2009/01/volume-rendering-101.html
//--------------------------------------------------------------------------------------


#pragma pack_matrix(row_major)


//--------------------------------------------------------------------------------------
// Constant buffer variables
//--------------------------------------------------------------------------------------

cbuffer cbEveryFrame : register(b0)
{
	matrix mWVP;
}


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

struct VSInput
{
	float4 pos : POSITION;
};


struct PSInput
{
	float4 pos : SV_POSITION;
	float4 tex : TEXCOORD0;		// Position in model coordinates
};


//--------------------------------------------------------------------------------------
// Vertex shaders
//--------------------------------------------------------------------------------------

PSInput ModelPositionVS(VSInput input)
{
	PSInput output;
	input.pos.w = 1.0f;
	output.pos = mul(mWVP, input.pos);

	// Pass position (in model coordinates) to texture coordinate register
	output.tex = input.pos;//0.5*(input.pos+1);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel shaders
//--------------------------------------------------------------------------------------

float4 ModelPositionPS(PSInput input) : SV_TARGET
{
	// Make use of linear interpolation
	return input.tex;
}
