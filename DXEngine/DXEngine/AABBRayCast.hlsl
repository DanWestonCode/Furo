//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
cbuffer cbEveryFrame : register(b0)
{
    matrix mWVP;
}

cbuffer CamBuffer : register(b0)
{
	float3 CameraPos;
	float padding1;
}

cbuffer ObjectBuffer : register(b1)
{
	float3 ObjectPos;
	float padding2;

	float3 ObjectScale;
	float padding3;
}

cbuffer FluidBuffer : register(b2)
{
	float Absoprtion;
	float padding4;

	int Samples;
	float padding5;
}

//--------------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------------
Texture3D<float> txVolume : register(t0);

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
// Sampler
//--------------------------------------------------------------------------------------
SamplerState samplerLinear : register(s0);

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
	float3 start, ds;
	float stepSize;
	CalculateRay(input, start, ds, stepSize);

	float alpha = 1.0f;

	for (int i = 0; i < Samples; ++i, start += ds)
	{
		float D = txVolume.SampleLevel(samplerLinear, start, 0);
		alpha *= 1.0f - saturate(D * stepSize * Absoprtion);

		if (alpha <= 0.01f) {
			break;
		}
	}
	return float4(1, 0, 1, 0.5) * (1.0f - alpha);
}