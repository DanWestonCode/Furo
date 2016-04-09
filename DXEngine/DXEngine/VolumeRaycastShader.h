/// <summary>
/// VolumeRanycastShader.h
///
/// About:
/// VolumeRanycastShader.h interfaces the raycast.hlsl shader and derives 
/// from the shader base class. The implementation is based from Dr.Christian B Mendl
/// Implementation found within the InitDevice() function
///
/// Dr.Christian B Mendl: http://christian.mendl.net/pages/software.html - See 'InitDevice' function in 'Volume Ray-Casting Renderer project'
/// </summary>
#ifndef VolumeRaycastShader_h__
#define VolumeRaycastShader_h__

#include "ShaderBase.h"

__declspec(align(16)) class VolumeRaycastShader : public ShaderBase
{
protected:
	struct WindowSizeBuffer
	{
		float windowSize[2];
		float dummy[2];
	};

public:
	VolumeRaycastShader();
	VolumeRaycastShader(const VolumeRaycastShader&);
	~VolumeRaycastShader();

	virtual HRESULT Initialize(ID3D11Device*, HWND, int, int);
	virtual void Shutdown();

	ID3D11Buffer* m_WindowSizeCB;
};

#endif // VolumeRaycastShader_h__
