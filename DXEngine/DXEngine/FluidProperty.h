#ifndef FluidProperty_h__
#define FluidProperty_h__
#include <DirectXMath.h>

#include <d3d11.h>
__declspec(align(16)) struct FluidProperty
{
	ID3D11Texture3D* m_Texture3D;
	ID3D11UnorderedAccessView* m_UAV;
	ID3D11ShaderResourceView* m_SRV;
	float buffer;
};

#endif // FluidProperty_h__
