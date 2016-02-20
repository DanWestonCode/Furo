#define NUM_THREADS 8

RWTexture3D<float4> _VorticityResult : register (u0);

Texture3D<float3> _Velocity : register (t0);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeVorticity( uint3 id : SV_DispatchThreadID )
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

	float3 vT = _Velocity[coordT];
	float3 vB = _Velocity[coordB];
	float3 vR = _Velocity[coordR];
	float3 vL = _Velocity[coordL];
	float3 vU = _Velocity[coordU];
	float3 vD = _Velocity[coordD];

	// using central differences: D0_x = (D+_x - D-_x) / 2
	float3 result = 0.5f * float3( (( vT.z - vB.z ) - ( vU.y - vD.y )) ,
								   (( vU.x - vD.x ) - ( vR.z - vL.z )) ,
								   (( vR.y - vL.y ) - ( vT.x - vB.x )) );

	float lresult = length(result);
	_VorticityResult[id] = float4(result, lresult);
}