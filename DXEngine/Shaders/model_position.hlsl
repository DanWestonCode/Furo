/// <summary>
/// model_positions.hlsl
///
/// About:
/// model_posiutions maps VO texture coords, Shader was taken from Dr.Christian B Mendl
/// project 'Volume Ray-Casting Renderer project'
///
/// References:
/// Dr.Christian B Mendl: http://christian.mendl.net/pages/software.html - See 'model_positions.hlsl' shader in 'Volume Ray-Casting Renderer project'
///</summary>
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
// Vertex shader
//--------------------------------------------------------------------------------------
PSInput ModelPositionVS(VSInput input)
{
	PSInput output;

	input.pos.w = 1.0f;

	output.pos = mul(mWVP, input.pos);

	// Pass position (in model coordinates) to texture coordinates
	output.tex = 0.5*(input.pos+1);

	return output;
}
//--------------------------------------------------------------------------------------
// Pixel shader
//--------------------------------------------------------------------------------------
float4 ModelPositionPS(PSInput input) : SV_TARGET
{
	return input.tex;
}
