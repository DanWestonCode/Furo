#define NUM_THREADS 8
cbuffer BuoyancyBuffer : register (b0) 
{
    float ambientTemperature;
	float buoyancy;
    float dt;
	float weight;
}

RWTexture3D<float3> _buoyancyResult : register(u0);

Texture3D<float3> _Velocity : register(t0);
Texture3D<float> _Density : register(t1);
Texture3D<float> _Temperature : register(t2);

[numthreads(NUM_THREADS,NUM_THREADS,NUM_THREADS)]
void ComputeBuoyancy (int3 id : SV_DispatchThreadID)
{
	float temperatureVal = _Temperature[id];
	float densityVal = _Density[id];

	float3 result = _Velocity[id];
	
	result += (dt * (temperatureVal) * buoyancy - (densityVal * weight) ) * float3(0,1,0);
	
	_buoyancyResult[id] = result;
}