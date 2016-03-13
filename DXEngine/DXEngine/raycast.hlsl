#pragma pack_matrix(row_major)

//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------

Texture3D<float> txVolume : register(t0);
Texture2D<float4> txPositionFront : register(t1);
Texture2D<float4> txPositionBack : register(t2);

//--------------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------------
uniform float3 _Size;

static const uint g_iMaxIterations = 128;

// Diagonal of a unit cube has length sqrt(3)
static const float g_fStepSize = sqrt(3.f) / g_iMaxIterations;

//--------------------------------------------------------------------------------------
// Sampler
//--------------------------------------------------------------------------------------

SamplerState samplerLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
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

cbuffer RenderProps : register(b1)
{
    float3 fluidCol;
    int iterations;
    float stepSize;
    float3 buffer;
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
	float3 worldPos : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex shader
//--------------------------------------------------------------------------------------
PSInput RayCastVS(VSInput input)
{
	PSInput output;

	input.pos.w = 1.0f;

	output.pos = mul(mWVP, input.pos);
	output.worldPos = mul(input.pos, mWVP).xyz;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel shader
//--------------------------------------------------------------------------------------
float4 RayCastPS(PSInput input) : SV_TARGET
{
	// Current pixel location on screen, used to sample position texture
	float2 tex = input.pos.xy * g_fInvWindowSize;
 
	// Read cube front and back face positions (in model coordinates) from texture
	float3 pos_front = txPositionFront.Sample(samplerLinear, tex);
	float3 pos_back  =  txPositionBack.Sample(samplerLinear, tex);
 
	float3 dir = normalize(pos_back - pos_front);

	// Single step: direction times delta step
    float3 step = stepSize * dir; //g_fStepSize * dir;

	// Current position
	float3 v = pos_front;

	// Accumulate result: value and transparency (alpha)
	float2 result = float2(0, 0);
 
    for (uint i = 0; i < iterations; i++)
	{
		float2 src = txVolume.Sample(samplerLinear, v).rr;

		// Reduce alpha to have a more transparent result
		src.y *= 0.06f;

		// Front to back blending
		result += ((1 - result.y)*src.y * src);

		// Advance the current position
		v += step;
	}
 
    return float4(result.r * fluidCol.r, result.r * fluidCol.g, result.r * fluidCol.b, result.y);
}