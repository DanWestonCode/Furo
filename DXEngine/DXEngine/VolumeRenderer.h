#ifndef VolumeRenderer_h__
#define VolumeRenderer_h__

#include "ModelShader.h"
#include "VolumeRaycastShader.h"
#include "RenderTexture.h"
#include "VolumeTexture.h"
#include "Cube.h"
#include "D3D.h"
#include "VolumeRenderer.h"
#include "AntTweakBar.h"
#include <stddef.h>

__declspec(align(16)) class VolumeRenderer
{
protected:
	struct MatrixBuffer
	{
		XMMATRIX mWVP;
	};

	struct CamBuffer
	{
		XMFLOAT3 CameraPos;
		float padding1;
	};

	struct ObjectBuffer
	{
		XMFLOAT3 ObjectPos;
		float padding2;

		XMFLOAT3 ObjectScale;
		float padding3;
	};

	struct FluidBuffer
	{
		float Absoprtion;
		float padding4;

		int Samples;
		float padding5;
	};

public:
	VolumeRenderer();
	VolumeRenderer(const VolumeRenderer&);
	~VolumeRenderer();

	void* operator new(size_t);
	void operator delete(void*);

	HRESULT Initialize(D3D*, HWND, int, int);
	void Shutdown();
	void Update(float, D3D*);
	void Render(D3D*);

private:
	void CreateSampler(ID3D11Device*);
protected:
	//objects
	ModelShader* m_ModelShader;
	VolumeRaycastShader* m_VolumeRaycastShader;
	RenderTexture* m_ModelFront;
	RenderTexture* m_ModelBack;
	VolumeTexture* m_VolumeTexture;
	Cube* m_cube;
	
	//sampler 
	ID3D11SamplerState* g_pSamplerLinear;

	XMMATRIX viewProj;

	ID3D11Buffer* m_CamBuffer;
	ID3D11Buffer* m_ObjectBuffer;
	ID3D11Buffer* m_FluidBuffer;
};
#endif // VolumeRenderer_h__
