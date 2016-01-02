#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <windows.h>
#include <list>

#include "D3D.h"
#include "camera.h"
#include "Quad.h"
#include "Bitmap.h"
#include "InputManager.h"
#include "TextureShader.h"
#include "RenderTexture.h"
#include "DebugWindow.h"
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

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float);

private:
	bool Render(float);
	bool RenderToTexture(float);
	bool RenderScene(float);

private:
	list<Object*> m_Objects;

	D3D* m_D3D;
	Camera* m_Camera;
	Quad* m_Quad;
	Furo* m_furo;
	Bitmap* m_MousePointer;
	TextureShader* m_TextureShader;
	TextureShader* m_TextureShader2;
	RenderTexture* m_RenderTexture;
	DebugWindow* m_DebugWindow;


	bool action = false;
};

#endif
