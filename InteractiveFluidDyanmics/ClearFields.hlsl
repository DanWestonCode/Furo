/// <summary>
/// ClearFields.hlsl
///
/// About:
/// ClearFields, clears all the Fluid fields for the gpu fluid sim
///
///</summary>
#define NUM_THREADS 8
RWTexture3D<float3> _VeloR : register(u0);
RWTexture3D<float3> _VeloW : register(u1);
RWTexture3D<float3> _DensR : register(u2);
RWTexture3D<float3> _DensW : register(u3);
RWTexture3D<float3> _VortR : register(u4);
RWTexture3D<float3> _TempR : register(u5);
RWTexture3D<float3> _TempW : register(u6);
RWTexture3D<float3> _DivR : register(u7);
RWTexture3D<float3> _PresR : register(u8);
RWTexture3D<float3> _PresW : register(u9);
RWTexture3D<float3> _TR : register(u10);
RWTexture3D<float3> _TW : register(u11);

[numthreads(NUM_THREADS, NUM_THREADS, NUM_THREADS)]
void ComputeClear(uint3 id : SV_DispatchThreadID)
{
    _VeloR[id] = 0;
    _VeloW[id] = 0;

    _DensR[id] = 0;
    _DensW[id] = 0;

    _VortR[id] = 0;

    _TempR[id] = 0;
    _TempW[id] = 0;

    _DivR[id] = 0;

    _PresR[id] = 0;
    _PresW[id] = 0;

    _TR[id] = 0;
    _TW[id] = 0;
}