
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <windows.h>
#include "D3D.h"

#include "camera.h"
#include "colorshader.h"
#include "textureshader.h"
#include "Quad.h"
#include "InputManager.h"


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

private:

	D3D* m_D3D;
	Camera* m_Camera;
	ColorShader* m_ColorShader;
	Quad* m_Quad;
	Furo* m_furo;
	TextureShader* m_TextureShader;

	bool action = false;
};

#endif
