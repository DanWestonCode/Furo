/// <summary>
/// Graphics.cpp
///
/// About:
/// This class is where all GameObjects/renderers/Simulations 
/// are initialized and run. This class is based from the 
/// GraphicsClass, class found in RasterTek tutorial 04.
///
/// RasterTek tutorial:
/// http://www.rastertek.com/dx11s2tut04.html
/// </summary>

#include "Graphics.h"
const UINT							m_fluidSize = 64;
Graphics::Graphics()
{
	m_D3D = nullptr;
	m_Quad = nullptr;
	m_fluidGPU = nullptr;
	m_fluid = nullptr;
	m_VolumeRenderer = nullptr;
	m_ClearBackBufferColor = nullptr;
}

Graphics::Graphics(const Graphics& other){}

Graphics::~Graphics(){}

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
	
	#pragma region Create D3D Object/Device
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
	#pragma endregion

	#pragma region DXEngine AnTweak Vars
	m_ClearBackBufferColor = new float;
	std::memset(m_ClearBackBufferColor, 0, sizeof(float) * 4);
	float ClearBackBuffer[4] = { 0.f, 0.f, 0.f, 1.f };
	TwAddSeparator(m_D3D->m_TwBar, "Engine", "");
	TwAddVarRW(m_D3D->m_TwBar, "Camera Position", TW_TYPE_DIR3F, &Camera::Instance()->m_pos, "");
	TwAddVarRW(m_D3D->m_TwBar, "Back Buffer", TW_TYPE_COLOR3F, &*m_ClearBackBufferColor, "");
	#pragma endregion

	/************************************/
	/*USE THIS TO CHANGE SIMULATION TYPE*/
	/************************************/
	m_simType = GPU3D;

	#pragma region FURO SIM INIT
	switch (m_simType)
	{
	case CPU2D:
		m_Quad = new Quad;
		m_Quad->Initialise(m_D3D, hwnd);
		Camera::Instance()->m_pos = XMFLOAT3(0.0f, 25.0f, -200);
		Camera::Instance()->m_LookAt = XMFLOAT3(50.f, 25.0f, 0.0f);
		break;
	case CPU3D:
		//Create new Volume Renderer and Initialize 
		m_VolumeRenderer = new VolumeRenderer;
		m_VolumeRenderer->Initialize(m_D3D, hwnd, m_D3D->m_ScreenWidth, m_D3D->m_ScreenHeight, false);
		m_VolumeTexture = new VolumeTexture;
		m_VolumeTexture->Initialize(m_D3D, m_fluidSize);
		break;
	case GPU3D:
		//Create new Volume Renderer and Initialize 
		m_VolumeRenderer = new VolumeRenderer;
		m_VolumeRenderer->Initialize(m_D3D, hwnd, m_D3D->m_ScreenWidth, m_D3D->m_ScreenHeight, true);
		#pragma region GPU Fluid
		m_fluidGPU = new FluidGPU;
		m_fluidGPU->Initialize(m_fluidSize, m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd);
		#pragma region AnTweak Bar vars
		TwStructMember _GPUFluidVars[] = {
			{ "Impulse Radius", TW_TYPE_FLOAT, offsetof(FluidGPU::SimulationVars, m_impulseRadius), "min=0.01 max=0.08 step=0.01" },
			{ "Density", TW_TYPE_FLOAT, offsetof(FluidGPU::SimulationVars, m_densityAmount), "min=0.1 max=10 step=0.1" },
			{ "Temperature", TW_TYPE_FLOAT, offsetof(FluidGPU::SimulationVars, m_TemperatureAmount), "min=1.0 max=1000 step=0.1" },
			{ "Decay", TW_TYPE_FLOAT, offsetof(FluidGPU::SimulationVars, m_decay), "min=0.0 max=100 step=0.1" },
			{ "Dissipation", TW_TYPE_FLOAT, offsetof(FluidGPU::SimulationVars, m_densityDissipation), "min=0.995 max=100 step=0.1" },
			{ "Ambient Temperature", TW_TYPE_FLOAT, offsetof(FluidGPU::SimulationVars, m_ambientTemperature), "min=0.995 max=1000 step=0.1" },
			{ "Buoyancy", TW_TYPE_FLOAT, offsetof(FluidGPU::SimulationVars, m_buoyancy), "min=0.995 max=100 step=0.1" },
			{ "Smoke Weight", TW_TYPE_FLOAT, offsetof(FluidGPU::SimulationVars, m_weight), "min=0.0125 max=100 step=0.1" },
			{ "Vorticity Strength", TW_TYPE_FLOAT, offsetof(FluidGPU::SimulationVars, m_VorticityStrength), "min=0.1 max=1000 step=0.1" }
		};
		TwAddVarRW(m_D3D->m_TwBar, "Simulation Properties", TwDefineStruct("Simulation", _GPUFluidVars, 9, sizeof(FluidGPU::SimulationVars), nullptr, nullptr), &m_fluidGPU->m_GPUFluidVars, NULL);
		#pragma endregion
		#pragma endregion
		break;
	}
	#pragma endregion

	return result;
}

void Graphics::Shutdown()
{
	//clean up objects
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}	

	if (m_fluid){
		m_fluid->ShutDown();
		delete m_fluid;
		m_fluid = nullptr;
	}

	if (m_fluidGPU){
		m_fluidGPU->Shutdown();
		delete m_fluidGPU;
		m_fluidGPU = nullptr;
	}

	if (m_VolumeRenderer){
		m_VolumeRenderer->Shutdown();
		delete m_VolumeRenderer;
		m_VolumeRenderer = nullptr;
	}

	if (m_Quad){
		m_Quad->Shutdown();
		delete m_Quad;
		m_Quad = nullptr;
	}

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
	//update scene camera
	Camera::Instance()->Update(dt);

	#pragma region FURO SIM UPDATE 
	//update pending on simulation type
	switch (m_simType)
	{
	case CPU2D:
		//RUN FURO 2D CPU SIM
		m_Quad->Update(dt, _hwnd);
		break;
	case CPU3D:
		//UPATE VOLTEXT/RUN FURO 3D CPU
		m_VolumeTexture->Update(m_D3D->GetDevice(), m_fluidSize, dt);
		m_VolumeRenderer->Update(dt, m_D3D);
		break;
	case GPU3D:
		//RUN FURO GPU SIM
		m_fluidGPU->Run(dt, m_D3D->GetDeviceContext());
		//UPDATE Volume Renderer
		m_VolumeRenderer->Update(dt, m_D3D);
		break;
	}
	#pragma endregion
}

bool Graphics::Render(float dt)
{	
	//Clear back buffer and depth stencil
	m_D3D->BeginScene(m_ClearBackBufferColor);
	//Render scene camera
	Camera::Instance()->Render();

	#pragma region FURO SIM RENDER
	//render pending on simulation type
	switch (m_simType)
	{
	case CPU2D:
		//RENDER QUAD/FURO 2D CPU SIM 
		m_Quad->Render(m_D3D);
		break;
	case CPU3D:
		//RENDER FURO 3D CPU
		m_VolumeRenderer->Render(m_D3D, m_VolumeTexture->m_ShaderResourceView);//pass SRV reference 
		break;
	case GPU3D:
		//RENDER FURO GPU SIM USING VOL RENDERER
		m_VolumeRenderer->Render(m_D3D, m_fluidGPU->m_DensitySRV[0]);//pass SRV reference 
		break;
	}
	#pragma endregion
		
	//Draw AnTweak
	TwDraw();
	//present the scene
	m_D3D->EndScene();
	return true;
}