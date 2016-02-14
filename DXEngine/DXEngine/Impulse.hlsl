#define NUM_THREADS 8

cbuffer ImpulseBuffer : register (b0) 
{
    float radius;
	float amount;
    float dt;
	float3 sourcePos;
    float3 padding1;
	float3 padding2;
}

RWTexture3D<float3> impulseResult : register(u0);
Texture3D<float3> impulseInitial : register(t0);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeImpulse(uint3 id : SV_DispatchThreadID)
{
	float3 pos = id - float3(16,1,16);
	float mag = pos.x*pos.x + pos.y*pos.y + pos.z*pos.z;
	float rad2 = radius*radius;
	 
	float3 _amount = exp(-mag/rad2) * amount * dt;
	impulseResult[id] = impulseInitial[id] + _amount;
}

