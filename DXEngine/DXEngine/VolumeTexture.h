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
#include "StamSolver3D.h"

using namespace DirectX;
using namespace Furo;
class VolumeTexture
{
public:
public:
	VolumeTexture();
	VolumeTexture(const VolumeTexture&);
	~VolumeTexture();

	HRESULT Initialize(D3D*, int);
	void Shutdown();

	void Update(ID3D11Device*, int, float);

public:
	//volume texture
	ID3D11Texture3D* m_Texture3D;
	ID3D11ShaderResourceView* m_ShaderResourceView;
	StamSolver3D* m_CPUFluid3D;
};

#endif // VolumeTexture_h__