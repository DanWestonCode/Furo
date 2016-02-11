#define NUM_THREADS 8

///PLEASE CHANGE THIS - TOO MUCH COPY AND PASTE///
// Constant buffers
cbuffer InputBufferGeneral : register (b0) {
    float fTimeStep;
    // Used for AdvectComputeShader, BuoyancyComputeShader	
	float fDensityBuoyancy;
    // Used for BuoyancyComputeShader
	float fDensityWeight;
    // Used for BuoyancyComputeShader
	float fVorticityStrength;
    // Used for VorticityComputeShader
	// 16 bytes //
};
cbuffer InputBufferAdvection : register (b1) {
    float fDissipation;
    // Used for AdvectComputeShader
	float fTimeStepModifier;
    float fDecay;
    float padding1;
    // pad to 16 bytes
}
///END COPY AND PASTE///

//SRV
Texture3D<float3> _AdvectionField : register (t0);
Texture3D<int>  _BoundaryConditions  : register (t1);
Texture3D<float3> _Velocity : register (t2);

//UAV
RWTexture3D<float3> _AdvectionTarget : register (u0);

// Samplers
SamplerState linearSampler : register (s0);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeAdvection(uint3 id : SV_DispatchThreadID)
{
    //if there's an obstacle bail
	if(_BoundaryConditions[id] > 0)
	{
        _AdvectionTarget[id] = float3(0,0,0);
        return;
    }

	uint3 _Size;
    //get dimensions of texture
	_Velocity.GetDimensions(_Size.x, _Size.y, _Size.z);
    //calc current texel
	uint3 idx = id.x + id.y*_Size.x + id.z*_Size.x*_Size.y;
    //back trace advection
	float3 prevPos = id - fTimeStepModifier * fTimeStep * _Velocity[id];
    prevPos = (prevPos+0.5f)/idx;
    float3 result = _AdvectionField.SampleLevel(linearSampler, prevPos, 0);
    float3 finalResult = result*fDissipation;
    if (fDecay > 0.0f)
	{
        finalResult.x = max(0, finalResult.x - fDecay);
    }

	_AdvectionTarget[id] = finalResult;
}