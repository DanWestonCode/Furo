#define NUM_THREADS 8

RWTexture3D<float> _pressureResult : register (u0);
Texture3D<float> _Pressure : register (t0);
Texture3D<int> _Obstacles : register (t1);
Texture3D<float> _Divergence : register (t2);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeJacobi( uint3 id : SV_DispatchThreadID )
{
	uint3 dimensions;
	//get dimensions of texture
	_Pressure.GetDimensions(dimensions.x, dimensions.y, dimensions.z);

	uint3 coordT = uint3(id.x, min(id.y+1,dimensions.y-1), id.z);
	uint3 coordB = uint3(id.x, max(id.y-1,0), id.z);
	uint3 coordR = uint3(min(id.x+1,dimensions.x-1), id.y, id.z);
	uint3 coordL = uint3(max(id.x-1,0), id.y, id.z);
	uint3 coordU = uint3(id.x, id.y, min(id.z+1,dimensions.z-1));
	uint3 coordD = uint3(id.x, id.y, max(id.z-1,0));

	float xC = _Pressure[id];

	float xT = _Pressure[coordT];
	float xB = _Pressure[coordB];
	float xR = _Pressure[coordR];
	float xL = _Pressure[coordL];
	float xU = _Pressure[coordU];
	float xD = _Pressure[coordD];

	if(_Obstacles[coordT] > 0.1) xT = xC;
    if(_Obstacles[coordB] > 0.1) xB = xC;
    
    if(_Obstacles[coordR] > 0.1) xR = xC;
    if(_Obstacles[coordL] > 0.1) xL = xC;
    
    if(_Obstacles[coordU] > 0.1) xU = xC;
    if(_Obstacles[coordD] > 0.1) xD = xC;

	// Sample divergence
	float bC = _Divergence[id];

	float final = (xL + xR + xB + xT + xU + xD - bC ) / 6;

	_pressureResult[id] = final;
}