/////////////
// GLOBALS //
/////////////
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
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	//input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(worldMatrix, input.position);

	// Store the input color for the pixel shader to use.
	output.color = input.color;
	output.color.a = input.color.a;
	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	return input.color;
}
