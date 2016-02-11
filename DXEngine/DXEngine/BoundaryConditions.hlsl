#define NUM_THREADS 8

Texture3D<int>  _BoundaryConditions  : register (t0);

RWTexture3D<int> _BoundaryConditionsRW : register (u0);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeBoundaryConditions(uint3 id : SV_DispatchThreadID)
{
	uint3 _Size;
	//get dimensions of texture
	_BoundaryConditionsRW.GetDimensions(_Size.x, _Size.y, _Size.z);
	//calc current texel
	uint3 idx = id.x + id.y*_Size.x + id.z*_Size.x*_Size.y;
    
    float obstacle = 0;
    
	//create boundary conditions
    if(id.x-1 < 0) obstacle = 1;
    if(id.x+1 > (int)_Size.x-1) obstacle = 1;
    
    if(id.y-1 < 0) obstacle = 1;
    if(id.y+1 > (int)_Size.y-1) obstacle = 1;
    
    if(id.z-1 < 0) obstacle = 1;
    if(id.z+1 > (int)_Size.z-1) obstacle = 1;
    
    _BoundaryConditionsRW[idx] = obstacle;
}
