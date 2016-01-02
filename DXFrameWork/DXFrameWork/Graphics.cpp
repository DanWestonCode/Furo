/****************************************************************/
/* Graphics class is where all graphics functionally is handled */
/*									                            */
/* This class is based on the Graphics class presented in the   */
/* Rastertek Tutorials                                          */
/*                                                              */
/* Created by Daniel Weston 18/12/2015                          */
/****************************************************************/

#include "Graphics.h"
#include <iostream>

Graphics::Graphics()
{
	m_D3D = nullptr;
	m_Camera = nullptr;
	m_Quad = nullptr;
	m_furo = nullptr;
	m_MousePointer = nullptr;
	m_TextureShader = nullptr;
}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the Direct3D object.
	m_D3D = new D3D;
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new Camera;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -250.0f);

	m_Quad = new Quad;
	if (!m_Quad)
	{
		return false;
	}
	m_Quad->Initialize(m_D3D->GetDevice(), L"../DXFrameWork/data/scifi_ground_01.dds", hwnd);

	m_Objects.push_back(m_Quad);

	m_furo = new Furo;
	if (!m_furo)
	{
		return false;
	}

	// Create the bitmap object.
	m_MousePointer = new Bitmap;
	if (!m_MousePointer)
	{
		return false;
	}
	// Initialize the bitmap object.
	m_MousePointer->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../DXFrameWork/data/scifi_wall_01.dds", 10, 10, hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
	//	return false;
	//}

	m_TextureShader = new TextureShader;
	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return result;
	}

	// Create the render to texture object.
	m_RenderTexture = new RenderTexture;
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	// Create the debug window object.
	m_DebugWindow = new DebugWindow;
	if (!m_DebugWindow)
	{
		return false;
	}

	// Initialize the debug window object.
	result = m_DebugWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, 100, 100);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}

	
	m_furo->Initialize(Furo::FluidField::TwoDimensional, 100, 0.1f);
	
	return true;
}

void Graphics::Shutdown()
{
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	if (m_Quad)
	{
		m_Quad->Shutdown();
		delete m_Quad;
		m_Quad = 0;
	}
	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	// Release the bitmap object.
	if (m_MousePointer)
	{
		m_MousePointer->Shutdown();
		delete m_MousePointer;
		m_MousePointer = 0;
	}
	// Release the debug window object.
	if (m_DebugWindow)
	{
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}

	// Release the render to texture object.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	//Release singletons
	TextureLoader::Instance()->DeleteInstance();

	return;
}

//at each frame render the graphics function
bool Graphics::Frame(float dt)
{
	bool result;

	// Render the graphics scene.
	result = Render(dt);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render(float dt)
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	int x, y;
	InputManager::Instance()->GetMouseLocation(x, y);
	bool result;

	m_Camera->Update(dt);
	m_Quad->Update(dt);
	m_furo->Run(dt);

	m_furo->GetFluid()->Clear();
	if (InputManager::Instance()->IsKeyDown(DIK_Q))
	{
		for (int i = 25; i < 50; i++)
		{
			m_furo->GetFluid()->SetDensity(i, 1, 1.0f);
		}
	}
	if (InputManager::Instance()->IsKeyDown(DIK_E))
	{
		for (int i = 25; i < 50; i++)
		{

			m_furo->GetFluid()->SetVelX(i, 1, 100.0f);
			m_furo->GetFluid()->SetVelY(i, 1, 100.0f);
			m_furo->GetFluid()->SetVelX(i, 2, 100.0f);
			m_furo->GetFluid()->SetVelY(i, 1, 100.0f);
		}

	}	

	// Render the entire scene to the texture first.
	result = RenderToTexture(dt);
	if (!result)
	{
		return false;
	}

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	// Render the scene as normal to the back buffer.
	result = RenderScene(dt);
	if (!result)
	{
		return false;
	}


	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Get the world, view, and ortho matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_DebugWindow->Render(m_D3D->GetDeviceContext(), 50, 50);
	// Render the debug window using the texture shader.
	m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), &worldMatrix, &viewMatrix,
		&orthoMatrix, m_RenderTexture->GetShaderResourceView(), 1.0f);
	
	m_MousePointer->Render(m_D3D->GetDeviceContext(), x, y, &worldMatrix, &viewMatrix, &orthoMatrix);
	m_TextureShader->Render(m_D3D->GetDeviceContext(), m_MousePointer->GetIndexCount(), &worldMatrix, &viewMatrix, &orthoMatrix, m_MousePointer->GetTexture(), 1.0f);	

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
	
	//// Generate the view matrix based on the camera's position.
	//m_Camera->Render();

	//// Get the world, view, and projection matrices from the camera and d3d objects.
	//m_D3D->GetWorldMatrix(worldMatrix);
	//m_Camera->GetViewMatrix(viewMatrix);
	//m_D3D->GetProjectionMatrix(projectionMatrix);
	//m_D3D->GetOrthoMatrix(orthoMatrix);
	//
	//// Turn off the Z buffer to begin all 2D rendering.
	//m_D3D->TurnZBufferOff();
	//// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//m_MousePointer->Render(m_D3D->GetDeviceContext(), x, y, &worldMatrix, &viewMatrix, &orthoMatrix);
	//// Render the model using the texture shader.
	//m_TextureShader->Render(m_D3D->GetDeviceContext(), m_MousePointer->GetIndexCount(), &worldMatrix, &viewMatrix, &orthoMatrix, m_MousePointer->GetTexture(), 1.0f);
	//m_D3D->TurnZBufferOn();
}

bool Graphics::RenderToTexture(float dt)
{
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = RenderScene(dt);
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderScene(float dt)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	

	m_Quad->UpdateTexture(m_furo->GetFluid()->GetDensity());

	//Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	for (list<Object *>::iterator it = m_Objects.begin(); it != m_Objects.end(); it++)
	{
		(*it)->Render(m_D3D->GetDeviceContext(), &worldMatrix, &viewMatrix, &projectionMatrix);
	}

	return true;
}


