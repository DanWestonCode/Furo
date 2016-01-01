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
	m_Bitmap = nullptr;
	m_TextureShader = nullptr;
	m_TextureShader2 = nullptr;
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

	m_furo = new Furo;
	if (!m_furo)
	{
		return false;
	}

	// Create the bitmap object.
	m_Bitmap = new Bitmap;
	if (!m_Bitmap)
	{
		return false;
	}
	// Initialize the bitmap object.
	m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../DXFrameWork/data/scifi_wall_01.dds", 10, 10, hwnd);
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

	m_TextureShader2 = new TextureShader;
	// Initialize the texture shader object.
	result = m_TextureShader2->Initialize(m_D3D->GetDevice(), hwnd);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return result;
	}

	
	m_furo->Initialize(Furo::FluidField::TwoDimensional, 100, 0.1f);
	
	return true;
}

void Graphics::Shutdown()
{
	if (m_TextureShader2)
	{
		m_TextureShader2->Shutdown();
		delete m_TextureShader2;
		m_TextureShader2 = 0;
	}
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
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
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
	m_Camera->Update(dt);
	m_Quad->Update(dt);

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;
	int x, y;
	InputManager::Instance()->GetMouseLocation(x, y);

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

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
	
	m_furo->Run(dt);

	m_Quad->UpdateTexture(m_furo->GetFluid()->GetDensity());

	//Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Quad->Render(m_D3D->GetDeviceContext(), &worldMatrix, &viewMatrix, &projectionMatrix);


	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	
	//// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();
	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Bitmap->Render(m_D3D->GetDeviceContext(), x, y, &worldMatrix, &viewMatrix, &orthoMatrix);
	// Turn the Z buffer back on now that all 2D rendering has completed.

	// Render the model using the texture shader.
	m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), &worldMatrix, &viewMatrix, &orthoMatrix, m_Bitmap->GetTexture(), 1.0f);

	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

