#define NUM_THREADS 8

Texture3D<float> _Pressure : register (t0);
Texture3D<float3> _Velocity : register (t1);
Texture3D<int> _BoundaryConditions : register (t2);

RWTexture3D<float3> _VelocityResult : register (u0);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeProjection(uint3 id : SV_DispatchThreadID)
{
	if (_BoundaryConditions[id] > 0)
	{
		_VelocityResult[id] = 0;
		return;
	}

	uint3 dimensions;
	_Pressure.GetDimensions(dimensions.x, dimensions.y, dimensions.z);

	uint3 coordT = uint3(id.x, min(id.y + 1, dimensions.y - 1), id.z);
	uint3 coordB = uint3(id.x, max(id.y - 1, 0), id.z);
	uint3 coordR = uint3(min(id.x + 1, dimensions.x - 1), id.y, id.z);
	uint3 coordL = uint3(max(id.x - 1, 0), id.y, id.z);
	uint3 coordU = uint3(id.x, id.y, min(id.z + 1, dimensions.z - 1));
	uint3 coordD = uint3(id.x, id.y, max(id.z - 1, 0));

	float pT = _Pressure[coordT];
	float pB = _Pressure[coordB];
	float pR = _Pressure[coordR];
	float pL = _Pressure[coordL];
	float pU = _Pressure[coordU];
	float pD = _Pressure[coordD];
	float pC = _Pressure[id];

	float3 vMask = float3(1, 1, 1);
	float3 obstV = float3(0, 0, 0);

	if (_BoundaryConditions[coordT]) { pT = pC; obstV.y = _Velocity[coordT].y; vMask.y = 0; }
	if (_BoundaryConditions[coordB]) { pB = pC; obstV.y = _Velocity[coordB].y; vMask.y = 0; }
	if (_BoundaryConditions[coordR]) { pR = pC; obstV.x = _Velocity[coordR].x; vMask.x = 0; }
	if (_BoundaryConditions[coordL]) { pL = pC; obstV.x = _Velocity[coordL].x; vMask.x = 0; }
	if (_BoundaryConditions[coordU]) { pU = pC; obstV.z = _Velocity[coordU].z; vMask.z = 0; }
	if (_BoundaryConditions[coordD]) { pD = pC; obstV.z = _Velocity[coordD].z; vMask.z = 0; }

	float3 grad = float3(pR - pL, pT - pB, pU - pD) * 0.5f;
	float3 oldV = _Velocity[id];
	float3 newV = oldV - grad;
	_VelocityResult[id] = newV * vMask + obstV;
}