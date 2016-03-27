#ifndef VolumeRenderer_h__
#define VolumeRenderer_h__

#include "ModelShader.h"
#include "VolumeRaycastShader.h"
#include "AABBVolumeRaycastShader.h"
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
	struct RenderProps
	{
		XMFLOAT3 fluidCol;
		int iterations;
		float stepSize;
		float absoprtion;
		float padding[2];
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
	void UpdateBuffers(D3D*);
	void Render(D3D*);

private:
	void CreateSampler(ID3D11Device*);
private:
	RenderProps m_RenderProps;
	RenderProps m_PrevProps;
protected:
	//objects
	ModelShader* m_ModelShader;
	VolumeRaycastShader* m_VolumeRaycastShader;
	RenderTexture* m_ModelFront;
	RenderTexture* m_ModelBack;
	VolumeTexture* m_VolumeTexture;
	FluidShader* m_fluidShader;
	Cube* m_cube;
	
	//sampler 
	ID3D11SamplerState* g_pSamplerLinear;

	XMMATRIX viewProj;

	//buffers
	ID3D11Buffer* m_RenderPropsBuffer;
};
#endif // VolumeRenderer_h__
