struct BoundaryConditions
{
	float3 x;
	float3 y;
};

RWStructuredBuffer<BoundaryConditions> _boundaryConditions;

[numthreads(16, 1, 1)]
void ComputeBoundaryConditions(int3 id : SV_DispatchThreadID)
{
	_boundaryConditions[id.x].x = float3(id.x,id.y,id.z);
}

