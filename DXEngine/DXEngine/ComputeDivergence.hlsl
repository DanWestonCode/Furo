#define NUM_THREADS 8

RWTexture3D<float> _DivergenceResult : register (u0);

Texture3D<float3> _Velocity : register (t0);
Texture3D<int> _BoundaryConditions : register (t1);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeDivergence( uint3 id : SV_DispatchThreadID ) 
{
	uint3 dimensions;
	//get dimensions of texture
	_Velocity.GetDimensions(dimensions.x, dimensions.y, dimensions.z);

	uint3 coordT = uint3(id.x, min(id.y+1,dimensions.y-1), id.z);
	uint3 coordB = uint3(id.x, max(id.y-1,0), id.z);
	uint3 coordR = uint3(min(id.x+1,dimensions.x-1), id.y, id.z);
	uint3 coordL = uint3(max(id.x-1,0), id.y, id.z);
	uint3 coordU = uint3(id.x, id.y, min(id.z+1,dimensions.z-1));
	uint3 coordD = uint3(id.x, id.y, max(id.z-1,0));

	// Find neighbouring velocities
	float3 vT = _Velocity[coordT];
	float3 vB = _Velocity[coordB];
	float3 vR = _Velocity[coordR];
	float3 vL = _Velocity[coordL];
	float3 vU = _Velocity[coordU];
	float3 vD = _Velocity[coordD];

	// Enforce boundaries
	float3 obstacleVelocity = float3(0,0,0);
    
    if(_BoundaryConditions[coordT] > 0.1) vT = obstacleVelocity;
    if(_BoundaryConditions[coordB] > 0.1) vB = obstacleVelocity;
    
    if(_BoundaryConditions[coordR] > 0.1) vR = obstacleVelocity;
    if(_BoundaryConditions[coordL] > 0.1) vL = obstacleVelocity;
    
    if(_BoundaryConditions[coordU] > 0.1) vU = obstacleVelocity;
    if(_BoundaryConditions[coordD] > 0.1) vD = obstacleVelocity;

	float result = 0.5f * (vR.x - vL.x + vT.y - vB.y + vU.z - vD.z);

	_DivergenceResult[id] = result;
}