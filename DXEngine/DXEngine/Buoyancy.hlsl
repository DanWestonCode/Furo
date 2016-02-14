#define NUM_THREADS 8
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

Texture3D<float> temperature : register (t0);
Texture3D<float> density : register (t1); 
Texture3D<float3> velocity : register (t2);

RWTexture3D<float3> buoyancyResult : register (u0); 

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
// Create upward force by using the temperature difference
void ComputeBuoyancy( uint3 i : SV_DispatchThreadID )
 {
	float temperatureVal = temperature[i];
	float densityVal = density[i];

	float3 result = velocity[i];

	result += (fTimeStep * (temperatureVal) * fDensityBuoyancy - (densityVal * fDensityWeight) ) * float3(0,1,0);

	buoyancyResult[i] = result;
}