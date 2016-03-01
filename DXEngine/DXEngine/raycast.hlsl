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

cbuffer CamBuffer : register(b1)
{
	float3 CameraPos;
	float padding1;
}

cbuffer ObjectBuffer : register(b2)
{
	float3 ObjectPos;
	float padding2;

	float3 ObjectScale;
	float padding3;
}

cbuffer FluidBuffer : register(b3)
{
	float Absoprtion;
	float padding4;

	int Samples;
	float padding5;
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

struct Ray 
{
	float3 origin;
	float3 dir;
};

struct AABB 
{
	float3 Min;
	float3 Max;
};

//--------------------------------------------------------------------------------------
// Helper Functions
//--------------------------------------------------------------------------------------

bool IntersectBox(Ray r, AABB aabb, out float t0, out float t1)
{
	float3 invR = 1.0f / r.dir;
	float3 tbot = invR * (aabb.Min - r.origin);
	float3 ttop = invR * (aabb.Max - r.origin);
	float3 tmin = min(ttop, tbot);
	float3 tmax = max(ttop, tbot);
	float2 t = max(tmin.xx, tmin.yz);
	t0 = max(t.x, t.y);
	t = min(tmax.xx, tmax.yz);
	t1 = min(t.x, t.y);
	return t0 <= t1;
}

void CalculateRay(PSInput input, out float3 start, out float3 stepVector, out float stepSize)
{
	//set up cam pos
	float3 RayPos = CameraPos;


	//create new Ray
	Ray r;
	r.origin = RayPos;
	r.dir = normalize(input.worldPos - RayPos);

	AABB aabb;
	aabb.Min = float3(-0.5f, -0.5f, -0.5f) * ObjectScale + ObjectPos;
	aabb.Max = float3(0.5f, 0.5f, 0.5f) * ObjectScale + ObjectPos;

	float tnear, tfar;
	IntersectBox(r, aabb, tnear, tfar);

	if (tnear < 0.0f) 
	{
		tnear = 0.0f;
	}

	float3 rayStart = r.origin + r.dir * tnear;
	float3 rayStop = r.origin + r.dir * tfar;

	//convert to texture space
	rayStart -= ObjectPos;
	rayStop -= ObjectPos;
	float3 halfScale = 0.5f * ObjectScale;
	rayStart = (rayStart + halfScale) / ObjectScale;
	rayStop = (rayStop + halfScale) / ObjectScale;

	float dist = distance(rayStop, rayStart);
	stepSize = dist / float(Samples);
	start = rayStart;
	stepVector = normalize(rayStop - rayStart) * stepSize;
}

//--------------------------------------------------------------------------------------
// Vertex shaders
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
// Pixel shaders
//--------------------------------------------------------------------------------------
//float4 RayCastPS(PSInput input) : SV_TARGET
//{
//	float3 start, ds;
//	float stepSize;
//	CalculateRay(input, start, ds, stepSize);
//
//	float alpha = 1.0f;
//
//	for (int i = 0; i < Samples; ++i, start += ds)
//	{
//		float D = txVolume.SampleLevel(samplerLinear, start, 0);
//		alpha *= 1.0f - saturate(D * stepSize * Absoprtion);
//
//		if (alpha <= 0.01f) {
//			break;
//		}
//	}
//	return float4(1, 0, 1, 0.5) * (1.0f - alpha);
//}

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
 
	for (uint i = 0; i < g_iMaxIterations; i++)
	{
		float2 src = txVolume.Sample(samplerLinear, v).rr;

		// Reduce alpha to have a more transparent result
		src.y *= 0.25;

		// Front to back blending
		result += ((1 - result.y)*src.y * src);

		// Advance the current position
		v += step;
	}
 
	return float4(result.r, result.r, result.r, (result.y));// *(1 - result.y);
}