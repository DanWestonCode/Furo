cbuffer MatrixBuffer
{
	matrix worldMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 tex : TEXCOORD0;
};

Texture2D a: register(t0);
SamplerState SampleType : register(s0);

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FluidVertexShader(VertexInputType input)
{
	PixelInputType output;

	//// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(worldMatrix, input.position);
	output.color = input.color;
	output.tex = 0.5*(input.position + 1);

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FluidPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	
	textureColor.r += 1;
	textureColor = a.Sample(SampleType, input.tex);
	

	return textureColor;
}