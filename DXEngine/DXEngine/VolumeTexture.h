#ifndef VolumeTexture_h__
#define VolumeTexture_h__

#include <d3d11.h>
#include<cstring>
#include <DirectXMath.h>
using namespace DirectX;
class VolumeTexture
{
public:
public:
	VolumeTexture();
	VolumeTexture(const VolumeTexture&);
	~VolumeTexture();

	HRESULT Initialize(ID3D11Device*, int);
	void Shutdown();
public:
	//volume texture
	ID3D11Texture3D* m_Texture3D;
	ID3D11ShaderResourceView* m_ShaderResourceView;
};

#endif // VolumeTexture_h__
