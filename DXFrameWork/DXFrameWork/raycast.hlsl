//--------------------------------------------------------------------------------------
// Ray cast through volume
//
// DirectX/HLSL implementation by
// Christian B. Mendl, 2012
//
// Reference:
// http://graphicsrunner.blogspot.de/2009/01/volume-rendering-101.html
//--------------------------------------------------------------------------------------


#pragma pack_matrix(row_major)


//--------------------------------------------------------------------------------------
// Textures and samplers
//--------------------------------------------------------------------------------------

Texture3D<float> txVolume : register(t0);

Texture2D<float4> txPositionFront : register(t1);
Texture2D<float4> txPositionBack  : register(t2);

SamplerState samplerLinear : register(s0);


//--------------------------------------------------------------------------------------
// Constants and constant buffer variables
//--------------------------------------------------------------------------------------

static const uint g_iMaxIterations = 128;

// Diagonal of a unit cube has length sqrt(3)
static const float g_fStepSize = sqrt(3.f)/g_iMaxIterations;

// for vertex shader
cbuffer cbEveryFrame : register(b0)
{
	matrix mWVP;
}

// for pixel shader
cbuffer cbImmutable : register(b0)
{
	float2 g_fInvWindowSize;
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
};


//--------------------------------------------------------------------------------------
// Vertex shaders
//--------------------------------------------------------------------------------------

PSInput RayCastVS(VSInput input)
{
	PSInput output;
	output.pos = mul(mWVP, input.pos);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel shaders
//--------------------------------------------------------------------------------------

float4 RayCastPS(PSInput input) : SV_TARGET
{
	// Current pixel location on screen, used to sample position texture
	float2 tex = input.pos.xy * g_fInvWindowSize;
 
	// Read cube front and back face positions (in model coordinates) from texture
	float3 pos_front = txPositionFront.Sample(samplerLinear, tex);
	float3 pos_back  =  txPositionBack.Sample(samplerLinear, tex);
 
	// Direction vector
	//float3 dir = pos_back - pos_front;
	//float ray_length = length(dir);	// ray length
	//dir /= ray_length;				// normalize direction
	float3 dir = normalize(pos_back - pos_front);

	// Number of iterations
	//uint numIter = min(ceil(ray_length/g_fStepSize), g_iMaxIterations);

	// Single step: direction times delta step
	float3 step = g_fStepSize * dir;

	// Current position
	float3 v = pos_front;

	// Accumulate result: value and transparency (alpha)
	float2 result = float2(0, 0);
 
	for (uint i = 0; i < /*numIter*/g_iMaxIterations; i++)
	{
		float2 src = txVolume.Sample(samplerLinear, v).rr;

		// Reduce alpha to have a more transparent result
		src.y *= 0.25;

		// Front to back blending
		result += (1 - result.y)*src.y * src;

		// Advance the current position
		v += step;
	}
 
	return float4(result.r, result.r, result.r, result.y);
}


//--------------------------------------------------------------------------------------
// Visualize front- and backface position textures
//--------------------------------------------------------------------------------------

float4 ShowFrontPositionPS(PSInput input) : SV_TARGET
{
	// Current pixel location on screen, used to sample position texture
	float2 tex = 4*input.pos.xy*g_fInvWindowSize;

	// Read cube front face positions (in model coordinates) from texture
	float3 pos_front = txPositionFront.Sample(samplerLinear, tex);

	return float4(pos_front, 1);
}

float4 ShowBackPositionPS(PSInput input) : SV_TARGET
{
	// Current pixel location on screen, used to sample position texture
	float2 tex = 4*(input.pos.xy*g_fInvWindowSize - float2(0.35, 0));

	// Read cube back face positions (in model coordinates) from texture
	float3 pos_back  =  txPositionBack.Sample(samplerLinear, tex);

	return float4(pos_back,  1);
}

float4 ShowDirectionPS(PSInput input) : SV_TARGET
{
	// Current pixel location on screen, used to sample position texture
	float2 tex = 4*(input.pos.xy*g_fInvWindowSize - float2(0.7, 0));

	// Read cube front and back face positions (in model coordinates) from texture
	float3 pos_front = txPositionFront.Sample(samplerLinear, tex);
	float3 pos_back  =  txPositionBack.Sample(samplerLinear, tex);

	return float4(pos_back - pos_front, 1);
}
