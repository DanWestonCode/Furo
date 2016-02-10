#define NUM_THREADS 8

RWTexture3D<float> _BoundaryConditions;

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeBoundaryConditions(uint3 id : SV_DispatchThreadID)
{
	_BoundaryConditions[id] = float3(1,1,1);
}

