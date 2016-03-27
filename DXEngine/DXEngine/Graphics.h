/// <summary>
/// Graphics.cpp
///
/// About:
/// This class is where all GameObjects/renderes/Simulations 
/// are initalized and run. This class is based from the 
/// GraphicsClass, class found in RasterTek tutorial 04.
///
/// RasterTek tutorial:
/// http://www.rastertek.com/dx11s2tut04.html
/// </summary>
/// <summary>
/// Graphics.cpp
///
/// About:
/// This class is where all GameObjects/renders/Simulations 
/// are initialised and run. This class is based from the 
/// GraphicsClass, class found in RasterTek tutorial 04.
///
/// RasterTek tutorial:
/// http://www.rastertek.com/dx11s2tut04.html
/// </summary>
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


	//Fix for 'warning C4316: object allocated on the heap may not be aligned 16'
	//This kept giving me access violation errors using XMMatrix calculations
	//functions discovered from:-
	//http://stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16
	void* operator new(size_t);
	void operator delete(void*);

	HRESULT Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float);

	HWND _hwnd;

private:
	void Update(float);
	bool Render(float);

private:
	//engine objects
	D3D* m_D3D;
	VolumeRenderer* m_VolumeRenderer;
	float* m_ClearBackBufferColor;
	Quad* m_Quad;
};

#endif