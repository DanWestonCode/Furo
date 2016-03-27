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
#include "Graphics.h"

Graphics::Graphics()
{
	m_D3D = nullptr;
	m_Quad = nullptr;
	m_VolumeRenderer = nullptr;
	m_ClearBackBufferColor = nullptr;
}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

//Fix for 'warning C4316: object allocated on the heap may not be aligned 16'
//This kept giving me access violation errors using XMMatrix calculations
//functions discovered from:-
//http://stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16
void* Graphics::operator new(size_t memorySize)
{
	return _aligned_malloc(memorySize, 16);
}

void Graphics::operator delete(void* memoryBlockPtr)
{
	_aligned_free(memoryBlockPtr);
	return;
}

HRESULT Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	_hwnd = hwnd;


	HRESULT result = S_OK;
	// Create the Direct3D object.
	m_D3D = new D3D;
	if (!m_D3D)
	{
		return S_FALSE;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return S_FALSE;
	}

	m_ClearBackBufferColor = new float;
	std::memset(m_ClearBackBufferColor, 0, sizeof(float) * 4);
	float ClearBackBuffer[4] = { 0.f, 0.f, 0.f, 1.f };
	TwAddSeparator(m_D3D->m_TwBar, "Engine", "");
	TwAddVarRW(m_D3D->m_TwBar, "Camera Position", TW_TYPE_DIR3F, &Camera::Instance()->m_pos, "");
	TwAddVarRW(m_D3D->m_TwBar, "Back Buffer", TW_TYPE_COLOR3F, &*m_ClearBackBufferColor, "");


	m_VolumeRenderer = new VolumeRenderer;
	m_VolumeRenderer->Initialize(m_D3D, hwnd, m_D3D->m_ScreenWidth, m_D3D->m_ScreenHeight);
	
	//m_Quad = new Quad;
	//m_Quad->Initialise(m_D3D, hwnd);

	return S_OK;
}

void Graphics::Shutdown()
{
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}	

	m_VolumeRenderer->Shutdown();
	delete m_VolumeRenderer;
	m_VolumeRenderer = nullptr;

	m_Quad->Shutdown();
	delete m_Quad;
	m_Quad = nullptr;

	Camera::Instance()->DeleteInstance();

	TwTerminate();

	return;
}

bool Graphics::Frame(float dt)
{
	bool result;

	Update(dt);
	
	result = Render(dt);

	if (!result)
	{
		return false;
	}

	return true;
}

void Graphics::Update(float dt)
{
	Camera::Instance()->Update(dt);
	//m_Quad->Update(dt, _hwnd);

	m_VolumeRenderer->Update(dt, m_D3D);
}

bool Graphics::Render(float dt)
{	
	m_D3D->BeginScene(m_ClearBackBufferColor);

	Camera::Instance()->Render();
	
	m_VolumeRenderer->Render(m_D3D);

	//m_Quad->Render(m_D3D);		

	TwDraw();
	m_D3D->EndScene();

	return true;
}