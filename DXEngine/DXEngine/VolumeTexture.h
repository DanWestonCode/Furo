#ifndef VolumeTexture_h__
#define VolumeTexture_h__

#include <d3d11.h>
#include<cstring>
#include <DirectXMath.h>
#include "Furo.h"
#include <iostream>

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
	void Update(ID3D11Device*, int, float);
public:
	//volume texture
	ID3D11Texture3D* m_Texture3D;
	ID3D11ShaderResourceView* m_ShaderResourceView;
	BYTE *_buffer;
	Furo* m_furo;
};

#endif // VolumeTexture_h__
