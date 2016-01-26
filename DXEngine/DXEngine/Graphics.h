#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <windows.h>
#include "D3D.h"

#include "RenderTexture.h"
#include "Quad.h"
#include "VolumeRenderer.h"
#include "Camera.h"


const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

__declspec(align(16)) class Graphics
{

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

private:
	//engine objects
	D3D* m_D3D;
	VolumeRenderer* m_VolumeRenderer;
	float result;
	Quad* m_Quad;
};

#endif