#define NUM_THREADS 8

cbuffer ConfinementBuffer : register (b0)
{
	float dt;
	float VorticityStrength;
	float padding1;
	float padding2;
}

Texture3D<int> _BoundaryConditions : register (t0);
Texture3D<float4> _Vorticity : register (t1);
Texture3D<float3> _Velocity : register (t3);

RWTexture3D<float3> _VelocityResult : register (u0);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeConfinement(uint3 id : SV_DispatchThreadID)
{
	if (_BoundaryConditions[id] > 0)
	{
		return;
	}

	uint3 dimensions;
	_Vorticity.GetDimensions(dimensions.x, dimensions.y, dimensions.z);

	uint3 coordT = uint3(id.x, min(id.y + 1, dimensions.y - 1), id.z);
	uint3 coordB = uint3(id.x, max(id.y - 1, 0), id.z);
	uint3 coordR = uint3(min(id.x + 1, dimensions.x - 1), id.y, id.z);
	uint3 coordL = uint3(max(id.x - 1, 0), id.y, id.z);
	uint3 coordU = uint3(id.x, id.y, min(id.z + 1, dimensions.z - 1));
	uint3 coordD = uint3(id.x, id.y, max(id.z - 1, 0));

	float omegaT = _Vorticity[coordT].w;
	float omegaB = _Vorticity[coordB].w;
	float omegaR = _Vorticity[coordR].w;
	float omegaL = _Vorticity[coordL].w;
	float omegaU = _Vorticity[coordU].w;
	float omegaD = _Vorticity[coordD].w;

	float3 omega = _Vorticity[id].xyz;

	float3 eta = 0.5f * float3(omegaR - omegaL, omegaT - omegaB, omegaU - omegaD);
	eta = normalize(eta + float3(0.001f, 0.001f, 0.001f));

	float3 force = dt * VorticityStrength * float3((eta.y * omega.z - eta.z * omega.y), (eta.z * omega.x - eta.x * omega.z), (eta.x * omega.y - eta.y * omega.x));
	
	_VelocityResult[id] = _Velocity[id] + force;
}