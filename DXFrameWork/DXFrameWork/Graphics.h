#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <windows.h>
#include <list>

#include "D3D.h"
#include "camera.h"
#include "Quad.h"
#include "Cube.h"
#include "Bitmap.h"
#include "InputManager.h"
#include "RenderTexture.h"
#include "DebugWindow.h"
#include "DrawData.h"
#include "Furo.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	HRESULT Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float);

private:
	void Update(float);
	bool Render(float);
	bool RenderToTexture(float);
	bool RenderScene(float);

private:
	list<Object*> m_Objects;

	D3D* m_D3D;
	Camera* m_Camera;
	Quad* m_Quad;
	Cube* m_Cube;
	Furo* m_furo;
	Bitmap* m_MousePointer;
	RenderTexture* m_RenderTexture;
	DebugWindow* m_DebugWindow;
	DrawData* m_DD;

	bool action = false;
};

#endif
