#define NUM_THREADS 8

cbuffer AdvectionBuffer : register (b0) 
{
    float dissipation;
	float dt;
    float decay;
    float forward;
}

//SRVS
Texture3D<float3> _AdvectionTargetRead : register (t0);
Texture3D<int>  _BoundaryConditions  : register (t1);
Texture3D<float3> _Velocity : register (t2);

//UAV
RWTexture3D<float3> _AdvectionTargetWrite : register (u0);

// Samplers
SamplerState linearSampler : register (s0);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeAdvection(uint3 id : SV_DispatchThreadID)
{
    //if there's an obstacle bail
	if(_BoundaryConditions[id] > 0)
	{
        _AdvectionTargetWrite[id] = float3(0,0,0);
        return;
    }

	uint3 _Size;
    //get dimensions of texture
	_Velocity.GetDimensions(_Size.x, _Size.y, _Size.z);

    //back trace advection
	float3 prevPos = id - (forward * dt * _Velocity[id]);
    prevPos = (prevPos+0.5f)/_Size;

    float3 result = _AdvectionTargetRead.SampleLevel(linearSampler, prevPos, 0);
    float3 finalResult = result*dissipation;
    if (decay > 0.0f)
	{
        finalResult.x = max(0, finalResult.x - decay);
    }

	_AdvectionTargetWrite[id] = finalResult;
}