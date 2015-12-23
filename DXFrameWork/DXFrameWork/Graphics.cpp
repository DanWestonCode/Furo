/***************************************/
/* Graphics class is where all graphics*/
/* functionally is handled             */
/* Created by Daniel Weston 18/12/2015 */
/***************************************/

#include "Graphics.h"
#include <iostream>

Graphics::Graphics()
{
	m_D3D = nullptr;
	m_Camera = nullptr;
	m_ColorShader = nullptr;
	m_Quad = nullptr;
	m_furo = nullptr;
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

	//// Create the model object.
	//m_Model = new VertexModel;
	//if (!m_Model)
	//{
	//	return false;
	//}

	//// Initialize the model object.
	//result = m_Model->Initialize(m_D3D->GetDevice());
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
	//	return false;
	//}

	m_Quad = new Quad;
	if (!m_Quad)
	{
		return false;
	}
	m_Quad->Initialize(m_D3D->GetDevice(), L"../DXFrameWork/data/scifi_ground_01.dds");

	m_furo = new Furo;
	if (!m_furo)
	{
		return false;
	}

	
	m_furo->Initialize(Furo::FluidField::TwoDimensional, 100, 0.1f);
	
	

	// Create the color shader object.
	m_ColorShader = new ColorShader;
	if (!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShader;
	if (!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}
	return true;
}

void Graphics::Shutdown()
{
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
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

	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_furo->GetFluid()->Clear();
	
	if (InputManager::Instance()->IsWPressed())
	{
		m_furo->GetFluid()->SetDensity(1, 1, 1.0f);
		m_furo->GetFluid()->SetVelX(1, 1, 500.0f);
		m_furo->GetFluid()->SetVelY(1, 1, 500.0f);
	}
	m_furo->Run(dt);

	m_Quad->UpdateTexture(m_furo->GetFluid()->GetDensity());

	m_D3D->TurnOnAlphaBlending();

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Quad->Render(m_D3D->GetDeviceContext());

	// Render the model using the color shader.
	/*result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Quad->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
	return false;
	}*/

	

	// Render the model using the texture shader.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Quad->GetIndexCount(), &worldMatrix, &viewMatrix, &projectionMatrix,
		m_Quad->GetTexture(), 1.0f);
	if (!result)
	{
		return false;
	}


	// Turn off alpha blending.
	m_D3D->TurnOffAlphaBlending();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

