/// <summary>
/// VolumeTexture.h
///
/// About:
/// VolumeTexture.h loads a RAW file into a ID3D11Texture3D object 
/// The implementation was guided by Dr.Christian B Mendl adaptation 
/// of volume rendering 101's implementation of the functionality
///
/// Dr.Christian B Mendl: http://christian.mendl.net/pages/software.html - See 'InitDevice' function in 'Volume Ray-Casting Renderer project'
/// Volume Rendering 101: http://graphicsrunner.blogspot.co.uk/2009/01/volume-rendering-101.html
/// </summary>
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

public:
	//volume texture
	ID3D11Texture3D* m_Texture3D;
	ID3D11ShaderResourceView* m_ShaderResourceView;
};

#endif // VolumeTexture_h__