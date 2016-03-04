/// <summary>
/// ComputeJacobi.hlsl
///
/// About:
/// ComputeDivergence - computes the divergence according to the finite difference formula http://http.developer.nvidia.com/GPUGems/elementLinks/tab38_1.jpg
/// Divergence is the first part of the projection step. 
///
/// Based from:
/// cFluid3D.hlsl (BuoyancyComputeShader) - Valentin Hinov - https://github.com/Morji/Fluid-Simulation-DirectX11
/// ComputeDivergence.compute - Scrawk Blog - https://scrawkblog.com/2014/01/09/gpu-gems-to-unity-3d-fluid-simulation/
/// </summary>
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

	//For the current cell get neighbouring positions
    uint3 LeftCell = uint3(max(0, id.x - 1), id.y, id.z);
    uint3 RightCell = uint3(min(id.x + 1, dimensions.x - 1), id.y, id.z);
	
    uint3 TopCell = uint3(id.x, min(id.y + 1, dimensions.y - 1), id.z);
    uint3 BottomCell = uint3(id.x, max(id.y - 1, 0), id.z);

    uint3 UpCell = uint3(id.x, id.y, min(id.z + 1, dimensions.z - 1));
    uint3 DownCell = uint3(id.x, id.y, max(id.z - 1, 0));

	float xC = _Pressure[id];

    float xT = _Pressure[TopCell];
    float xB = _Pressure[BottomCell];
    float xR = _Pressure[RightCell];
    float xL = _Pressure[LeftCell];
    float xU = _Pressure[UpCell];
    float xD = _Pressure[DownCell];

    if (_Obstacles[TopCell] > 0.1)
        xT = xC;
    if (_Obstacles[BottomCell] > 0.1)
        xB = xC;
    
    if (_Obstacles[RightCell] > 0.1)
        xR = xC;
    if (_Obstacles[LeftCell] > 0.1)
        xL = xC;
    
    if (_Obstacles[UpCell] > 0.1)
        xU = xC;
    if (_Obstacles[DownCell] > 0.1)
        xD = xC;

	// Sample divergence
	float bC = _Divergence[id];

	float final = (xL + xR + xB + xT + xU + xD - bC ) / 6;

	_pressureResult[id] = final;
}