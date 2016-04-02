#ifndef VolumeTexture_h__
#define VolumeTexture_h__

#include <d3d11.h>
#include<cstring>
#include <DirectXMath.h>
#include "D3D.h"
#include <iostream>

using namespace DirectX;
class VolumeTexture
{
public:
public:
	VolumeTexture();
	VolumeTexture(const VolumeTexture&);
	~VolumeTexture();

	HRESULT Initialize(D3D*, int);
	void Shutdown();
	void Update(ID3D11Device*, ID3D11DeviceContext*, int, float);
	void Render(ID3D11DeviceContext*);

public:
	//volume texture
	ID3D11Texture3D* m_Texture3D;
	ID3D11ShaderResourceView* m_ShaderResourceView;
};

#endif // VolumeTexture_h__
