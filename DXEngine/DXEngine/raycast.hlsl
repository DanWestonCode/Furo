#pragma pack_matrix(row_major)

//--------------------------------------------------------------------------------------
// Textures and samplers
//--------------------------------------------------------------------------------------

Texture3D<float> txVolume : register(t0);

uniform float3 _Size;

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

float SampleBilinear(StructuredBuffer<float> buffer, float3 uv, float3 size)
{
	uv = saturate(uv);
	uv = uv * (size-1.0);
				
	int x = uv.x;
	int y = uv.y;
	int z = uv.z;
				
	int X = size.x;
	int XY = size.x*size.y;
				
	float fx = uv.x-x;
	float fy = uv.y-y;
	float fz = uv.z-z;
				
	int xp1 = min(_Size.x-1, x+1);
	//int yp1 = min(_Size.y-1, y+1);
	//int zp1 = min(_Size.z-1, z+1);
				
	//float x0 = buffer[x+y*X+z*XY] * (1.0f-fx) + buffer[xp1+y*X+z*XY] * fx;
	//float x1 = buffer[x+y*X+zp1*XY] * (1.0f-fx) + buffer[xp1+y*X+zp1*XY] * fx;
				
	//float x2 = buffer[x+yp1*X+z*XY] * (1.0f-fx) + buffer[xp1+yp1*X+z*XY] * fx;
	//float x3 = buffer[x+yp1*X+zp1*XY] * (1.0f-fx) + buffer[xp1+yp1*X+zp1*XY] * fx;
				
	//float z0 = x0 * (1.0f-fz) + x1 * fz;
	//float z1 = x2 * (1.0f-fz) + x3 * fz;
				
	//return z0 * (1.0f-fy) + z1 * fy;	
	return 1.0f;			
}

