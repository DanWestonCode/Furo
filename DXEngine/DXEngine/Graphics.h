#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <windows.h>
#include "D3D.h"

#include "ModelShader.h"
#include "VolumeRaycastShader.h"


const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

__declspec(align(16)) class Graphics
{

protected:
	struct MatrixBuffer
	{
		XMMATRIX mWVP;
	};

public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	void* operator new(size_t);
	void operator delete(void*);

	HRESULT Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float);

private:
	void Update(float);
	bool Render(float);

	//create new texture
	void CreateRenderTexture(ID3D11Device*);
	//create sampler
	void CreateSampler(ID3D11Device*);
	//load volume texture
	void LoadVolume(ID3D11Device*);
	//create cube
	void CreateCube(ID3D11Device*);


private:
	//engine objects
	D3D* m_D3D;
	ShaderBase* m_shader;
	ModelShader* m_ModelShader;
	VolumeRaycastShader* m_VolumeRaycastShader;

	//render textures
	ID3D11Texture2D* ModelTexture2DFront;
	ID3D11ShaderResourceView* ModelShaderResourceViewFront;
	ID3D11RenderTargetView*	ModelRenderTargetViewFront;
	ID3D11Texture2D* ModelTexture2DBack;
	ID3D11ShaderResourceView* ModelShaderResourceViewBack;
	ID3D11RenderTargetView*	ModelRenderTargetViewBack;
	//sampler 
	ID3D11SamplerState* g_pSamplerLinear;
	//volume texture
	ID3D11Texture3D* VolumeTexture3D;
	ID3D11ShaderResourceView* VolumeResourceView;
	//vertex and index buffers
	ID3D11Buffer* CubeVB;
	ID3D11Buffer* CubeIB;

	XMMATRIX viewProj;	
};

#endif