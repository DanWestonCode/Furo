/// <summary>
/// FluidGPU.cpp
///
/// About:
/// FluidGPU.cpp contains the implementation for compiling and running
/// the shader based (GPU) fluid simulation. The implementation found within this
/// class was partially guided by Scrawks 'SmokeFluidSim.cs' class and  Valentin Hinov 'Fluid3DCalculator.cpp' 
/// class. Valentin Hinov's creation of Textures, UAV's and SRV's within 'FluidResource.cpp was
/// utilized with the creation of resources within this class. The Update function containing the simulation steps 
/// was taken from Keenan Crane GPU gems chapter.
///
/// Based from:
/// Fluid3DCalculator.cpp - Valentin Hinov - https://github.com/Morji/Fluid-Simulation-DirectX11
/// FluidResources.cpp - Valentin Hinov - https://github.com/Morji/Fluid-Simulation-DirectX11
/// SmokeFluidSim.cs - Scrawk Blog - https://scrawkblog.com/2014/01/09/gpu-gems-to-unity-3d-fluid-simulation/
/// GPU Gems update step - Keenan Crane - http://http.developer.nvidia.com/GPUGems3/gpugems3_ch30.html
/// </summary>
#define READ 0
#define WRITE 1
#define NUM_THREADS (UINT)8

#include "FluidGPU.h"

using namespace Furo;
using namespace std;
void* FluidGPU::operator new(size_t memorySize)
{
	return _aligned_malloc(memorySize, 16);

}

void FluidGPU::operator delete(void* memoryBlockPtr)
{
	_aligned_free(memoryBlockPtr);
	return;
}

void FluidGPU::Initialize(int _size, ID3D11Device* _device, ID3D11DeviceContext* _deviceContext, HWND _hwnd)
{
	HRESULT result;
	//set up Fluid size
	FluidSize = _size;
	//compile all program shaders
	CompileShaders(_device, _hwnd);
	//create all SRVS/UAVs/Textures 
	CreateResources(_device);

#pragma region Buffers
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	//ImpulseBuffer
	bufferDesc.ByteWidth = sizeof(ImpulseBuffer);
	_device->CreateBuffer(&bufferDesc, NULL, &m_DensityBuffer);

	//AdvectionBuffer
	bufferDesc.ByteWidth = sizeof(AdvectionBuffer);
	_device->CreateBuffer(&bufferDesc, NULL, &m_AdvectionBuffer);

	//Buoyancy Buffer
	bufferDesc.ByteWidth = sizeof(BuoyancyBuffer);
	_device->CreateBuffer(&bufferDesc, NULL, &m_BuoyancyBuffer);

	//Confinement
	bufferDesc.ByteWidth = sizeof(ConfinementBuffer);
	_device->CreateBuffer(&bufferDesc, NULL, &m_ConfinementBuffer);
#pragma endregion

#pragma region Create Sampler
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Create the texture sampler state.
	result = _device->CreateSamplerState(&samplerDesc, &m_Sampler);
#pragma endregion	

#pragma region Default Vars for Fluid Simulation
	m_GPUFluidVars.m_impulseRadius = 0.04f;
	m_GPUFluidVars.m_densityAmount = 1.0f;
	m_GPUFluidVars.m_TemperatureAmount = 1.0f;
	m_GPUFluidVars.m_decay = 0.0f;
	m_GPUFluidVars.m_temperatureDissipation = 0.995f;
	m_GPUFluidVars.m_velocityDissipation = 0.995f;
	m_GPUFluidVars.m_densityDissipation = 0.995f;
	m_GPUFluidVars.m_ambientTemperature = 0.995f;
	m_GPUFluidVars.m_buoyancy = 1.0f;
	m_GPUFluidVars.m_weight = 0.0125f;
	m_GPUFluidVars.m_VorticityStrength = 1.0f;
#pragma endregion

	//compute boundary conditions once
	ComputeBoundaryConditions(_deviceContext);
}

//Loads and compiles all CS shaders for fluid sim
void FluidGPU::CompileShaders(ID3D11Device* _device, HWND _hwnd)
{
	HRESULT result;
	ID3DBlob* blob = nullptr;

#pragma region Boundary Conditions
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ComputeBoundaryConditions.hlsl", "ComputeBoundaryConditions", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not initialize '../Shaders/ComputeBoundaryConditions.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_BoundaryConditionsCS);
#pragma endregion

#pragma region Advection
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ComputeAdvection.hlsl", "ComputeAdvection", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not initialize '../Shaders/ComputeAdvection.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_AdvectionCS);
#pragma endregion

#pragma region Buoyancy
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ComputeBuoyancy.hlsl", "ComputeBuoyancy", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not initialize '../Shaders/ComputeBuoyancy.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_BuoyancyCS);
#pragma endregion

#pragma region Impulse
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ComputeImpulse.hlsl", "ComputeImpulse", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not initialize '../Shaders/ComputeImpulse.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_ImpulseCS);
#pragma endregion

#pragma region Vorticity
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ComputeVorticity.hlsl", "ComputeVorticity", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not initialize '../Shaders/ComputeVorticity.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_VorticityCS);
#pragma endregion

#pragma region Confinement
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ComputeConfinement.hlsl", "ComputeConfinement", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not locate '../Shaders/ComputeConfinement.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_ConfinementCS);
#pragma endregion

#pragma region Divergence
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ComputeDivergence.hlsl", "ComputeDivergence", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not locate '../Shaders/ComputeDivergence.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_DivergenceCS);
#pragma endregion

#pragma region Jacobi
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ComputeJacobi.hlsl", "ComputeJacobi", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not locate '../Shaders/ComputeJacobi.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_JacobiCS);
#pragma endregion

#pragma region Projection
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ComputeProjection.hlsl", "ComputeProjection", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not locate '../Shaders/ComputeProjection.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_ProjectionCS);
#pragma endregion

#pragma region Clear
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../Shaders/ClearFields.hlsl", "ComputeClear", "cs_5_0", &blob);
	if (FAILED(result))
	{
		MessageBox(_hwnd, L"Could not locate '../Shaders/ClearFields.hlsl'", L"Error", MB_OK);
	}
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_ClearCS);
#pragma endregion

	blob->Release();
}

//Init and create Texture3D/SRVs/UAVs
void FluidGPU::CreateResources(ID3D11Device* _device)
{
	HRESULT result;
	D3D11_BUFFER_DESC inputDesc;
	D3D11_SUBRESOURCE_DATA vinitData;

#pragma region Texture3D Desc
	D3D11_TEXTURE3D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE3D_DESC));
	textureDesc.Width = (UINT)FluidSize;
	textureDesc.Height = (UINT)(FluidSize * 2);
	textureDesc.Depth = (UINT)FluidSize;
	textureDesc.MipLevels = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
#pragma endregion

#pragma region Boundary Conditions
	textureDesc.Format = DXGI_FORMAT_R8_SINT;
	//Create Texture3D
	result = (_device->CreateTexture3D(&textureDesc, NULL, &m_BoundaryConditions));
	//Create SRV
	result = (_device->CreateShaderResourceView(m_BoundaryConditions, NULL, &m_BoundaryConditionsSRV));
	//Create UAV
	result = (_device->CreateUnorderedAccessView(m_BoundaryConditions, NULL, &m_BoundaryConditionsUAV));
#pragma endregion

#pragma region Vorticity
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	//Create Texture3D
	result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Vorticity));
	//Create SRV
	result = (_device->CreateShaderResourceView(m_Vorticity, NULL, &m_VorticitySRV));
	//Create UAV
	result = (_device->CreateUnorderedAccessView(m_Vorticity, NULL, &m_VorticityUAV));
#pragma endregion

#pragma region Divergence
	textureDesc.Format = DXGI_FORMAT_R16_FLOAT;

	result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Divergence));
	//Create SRV
	result = (_device->CreateShaderResourceView(m_Divergence, NULL, &m_DivergenceSRV));
	//Create UAV
	result = (_device->CreateUnorderedAccessView(m_Divergence, NULL, &m_DivergenceUAV));
#pragma endregion

	for (int i = 0; i < 2; i++)
	{
#pragma region Pressure
		textureDesc.Format = DXGI_FORMAT_R16_FLOAT;

		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Pressure[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_Pressure[i], NULL, &m_PressureSRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_Pressure[i], NULL, &m_PressureUAV[i]));
#pragma endregion

#pragma region Density
		textureDesc.Format = DXGI_FORMAT_R16_FLOAT;

		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Density[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_Density[i], NULL, &m_DensitySRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_Density[i], NULL, &m_DensityUAV[i]));
#pragma endregion

#pragma region Velocity
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Velocity[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_Velocity[i], NULL, &m_VelocitySRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_Velocity[i], NULL, &m_VelocityUAV[i]));
#pragma endregion

#pragma region Temperature
		textureDesc.Format = DXGI_FORMAT_R16_FLOAT;

		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Temperature[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_Temperature[i], NULL, &m_TemperatureSRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_Temperature[i], NULL, &m_TemperatureUAV[i]));
#pragma endregion

#pragma region Temp
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_TempTexture[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_TempTexture[i], NULL, &m_TempSRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_TempTexture[i], NULL, &m_TempUAV[i]));
#pragma endregion
	}
}

//Release and delete all 
void FluidGPU::Shutdown()
{
#pragma region Shutdown and release Shaders
	m_BoundaryConditionsCS->Release();
	delete m_BoundaryConditionsCS;
	m_AdvectionCS->Release();
	delete m_AdvectionCS;
	m_BuoyancyCS->Release();
	delete m_BuoyancyCS;
	m_ImpulseCS->Release();
	delete m_ImpulseCS;
	m_VorticityCS->Release();
	delete m_VorticityCS;
	m_ConfinementCS->Release();
	delete m_ConfinementCS;
	m_DivergenceCS->Release();
	delete m_DivergenceCS;
	m_JacobiCS->Release();
	delete m_JacobiCS;
	m_ProjectionCS->Release();
	delete m_ProjectionCS;
#pragma endregion

#pragma region Shutdown and release buffers
	//shutdown and release buffers
	m_DensityBuffer->Release();
	delete m_DensityBuffer;
	m_ImpulseBuffer->Release();
	delete m_ImpulseBuffer;
	m_AdvectionBuffer->Release();
	delete m_AdvectionBuffer;
	m_BuoyancyBuffer->Release();
	delete m_BuoyancyBuffer;
	m_ConfinementBuffer->Release();
	delete m_ConfinementBuffer;
#pragma endregion

#pragma region Shutdown and release Textures/UAVs/SRVs
	//iterate for arrays
	for (int i = 0; i < 2; i++)
	{
#pragma region Density
		m_Density[i]->Release();
		delete m_Density[i];
		m_DensityUAV[i]->Release();
		delete m_DensityUAV[i];
		m_DensitySRV[i]->Release();
		delete m_DensityUAV[i];
#pragma endregion
#pragma region Velocity
		m_Velocity[i]->Release();
		delete m_Velocity[i];
		m_VelocityUAV[i]->Release();
		delete m_VelocityUAV[i];
		m_VelocitySRV[i]->Release();
		delete m_VelocitySRV[i];
#pragma endregion
#pragma region Temperature
		m_Temperature[i]->Release();
		delete m_Temperature[i];
		m_TemperatureUAV[i]->Release();
		delete m_TemperatureUAV[i];
		m_TemperatureSRV[i]->Release();
		delete m_TemperatureSRV[i];
#pragma endregion
#pragma region Pressure
		m_Temperature[i]->Release();
		delete m_Temperature[i];
		m_TemperatureUAV[i]->Release();
		delete m_TemperatureUAV[i];
		m_TemperatureSRV[i]->Release();
		delete m_TemperatureSRV[i];
#pragma endregion
	}
#pragma region Boundary Conditions
	m_BoundaryConditions->Release();
	delete m_BoundaryConditions;
	m_BoundaryConditionsUAV->Release();
	delete m_BoundaryConditionsUAV;
	m_BoundaryConditionsSRV->Release();
	delete m_BoundaryConditionsSRV;
#pragma endregion
#pragma region Vorticity 
	m_Vorticity->Release();
	delete m_Vorticity;
	m_VorticityUAV->Release();
	delete m_VorticityUAV;
	m_VorticitySRV->Release();
	delete m_VorticitySRV;
#pragma endregion
#pragma region Divergence 
	m_Divergence->Release();
	delete m_Divergence;
	m_DivergenceUAV->Release();
	delete m_DivergenceUAV;
	m_DivergenceSRV->Release();
	delete m_DivergenceSRV;
#pragma endregion
#pragma endregion
}

//Called once per frame - updates sim
void FluidGPU::Run(float _dt, ID3D11DeviceContext* _deviceContext)
{
	m_timeStep = 0.1f;

	ComputeAdvection(_deviceContext, m_TemperatureUAV[WRITE], m_TemperatureSRV[READ], m_GPUFluidVars.m_temperatureDissipation, 1);
	swap(m_TemperatureUAV[READ], m_TemperatureUAV[WRITE]);
	swap(m_TemperatureSRV[READ], m_TemperatureSRV[WRITE]);

	ComputeAdvection(_deviceContext, m_DensityUAV[WRITE], m_DensitySRV[READ], m_GPUFluidVars.m_densityDissipation, 1);
	swap(m_DensityUAV[READ], m_DensityUAV[WRITE]);
	swap(m_DensitySRV[READ], m_DensitySRV[WRITE]);

	ComputeAdvection(_deviceContext, m_VelocityUAV[WRITE], m_VelocitySRV[READ], m_GPUFluidVars.m_velocityDissipation, 1);
	swap(m_VelocityUAV[READ], m_VelocityUAV[WRITE]);
	swap(m_VelocitySRV[READ], m_VelocitySRV[WRITE]);

	ComputeBuoyancy(_deviceContext);
	swap(m_VelocityUAV[READ], m_VelocityUAV[WRITE]);
	swap(m_VelocitySRV[READ], m_VelocitySRV[WRITE]);

	ComputeImpulse(_deviceContext, m_DensityUAV[WRITE], m_DensitySRV[READ], m_GPUFluidVars.m_densityAmount);
	swap(m_DensityUAV[READ], m_DensityUAV[WRITE]);
	swap(m_DensitySRV[READ], m_DensitySRV[WRITE]);

	ComputeImpulse(_deviceContext, m_TemperatureUAV[WRITE], m_TemperatureSRV[READ], m_GPUFluidVars.m_TemperatureAmount);
	swap(m_TemperatureUAV[READ], m_TemperatureUAV[WRITE]);
	swap(m_TemperatureSRV[READ], m_TemperatureSRV[WRITE]);

	ComputeVorticity(_deviceContext);
	ComputeConfinement(_deviceContext);
	swap(m_VelocityUAV[READ], m_VelocityUAV[WRITE]);
	swap(m_VelocitySRV[READ], m_VelocitySRV[WRITE]);

	ComputeDivergence(_deviceContext);

	ComputeJacobi(_deviceContext);

	ComputeProjection(_deviceContext);
	swap(m_VelocityUAV[READ], m_VelocityUAV[WRITE]);
	swap(m_VelocitySRV[READ], m_VelocitySRV[WRITE]);
}

/// <summary>
/// Creates a 'solid' walls around fluid
/// containing it within the texture
/// compute shader.
/// </summary>
void FluidGPU::ComputeBoundaryConditions(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_BoundaryConditionsCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_BoundaryConditionsUAV, 0);
	//bind SRV
	_deviceContext->CSSetShaderResources(0, 1, &m_VelocitySRV[READ]);
	//Dispatch Shader
	_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);
	//unbind UAV
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

/// <summary>
/// Uses Stams(1999) back trace Advection scheme to Advect
/// quantities over the velocity field
/// using the the ComputeAdvection.hlsl 
/// compute shader.
/// </summary>
void FluidGPU::ComputeAdvection(ID3D11DeviceContext* _deviceContext, ID3D11UnorderedAccessView* _TargetWrite, ID3D11ShaderResourceView* _TargetRead, float _Dissipation, float _Forward)
{
#pragma region Update Advection Buffer
	AdvectionBuffer* dataPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = _deviceContext->Map(m_AdvectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (AdvectionBuffer*)mappedResource.pData;
	dataPtr->dissipation = _Dissipation;
	dataPtr->decay = 0.0f;
	dataPtr->dt = m_timeStep;
	dataPtr->forward = _Forward;
	_deviceContext->Unmap(m_AdvectionBuffer, 0);
#pragma endregion 

	//target Advection shader
	_deviceContext->CSSetShader(m_AdvectionCS, nullptr, 0);

	//Set Advection shader buffer to the AdvectionBuffer
	_deviceContext->CSSetConstantBuffers(0, 1, &m_AdvectionBuffer);

	//set sampler
	_deviceContext->CSSetSamplers(0, 1, &m_Sampler);

	//bind the UAV target for the Advection shader
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &_TargetWrite, 0);

	//Set the field to advect against
	ID3D11ShaderResourceView *const SRV[3] = { _TargetRead, m_BoundaryConditionsSRV, m_VelocitySRV[READ] };
	_deviceContext->CSSetShaderResources(0, 3, SRV);

	//Dispatch Compute shader to GPU
	_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);

	//unbind SRVs
	ID3D11ShaderResourceView* nullSRV[3] = { NULL, NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 3, nullSRV);
	//unbind UAVs
	ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	// Unbind Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidGPU::ComputeBuoyancy(ID3D11DeviceContext* _deviceContext)
{
#pragma region Update Buoyancy Buffer
	BuoyancyBuffer* dataPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = _deviceContext->Map(m_BuoyancyBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (BuoyancyBuffer*)mappedResource.pData;
	dataPtr->ambientTemperature = m_GPUFluidVars.m_ambientTemperature;
	dataPtr->buoyancy = m_GPUFluidVars.m_buoyancy;
	dataPtr->dt = m_timeStep;
	dataPtr->weight = m_GPUFluidVars.m_weight;
	_deviceContext->Unmap(m_BuoyancyBuffer, 0);
#pragma endregion 

	//target Buoyancy shader
	_deviceContext->CSSetShader(m_BuoyancyCS, nullptr, 0);

	//set the ConstantBuffer to be the Buoyancy buffer
	_deviceContext->CSSetConstantBuffers(0, 1, &m_BuoyancyBuffer);

	//bind the target as the UAV for the Buoyancy shader
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_VelocityUAV[WRITE], 0);

	//Set SRvs
	ID3D11ShaderResourceView *const SRV[3] = { m_VelocitySRV[READ], m_DensitySRV[READ], m_TemperatureSRV[READ] };
	_deviceContext->CSSetShaderResources(0, 3, SRV);

	//Dispatch compute shader to GPU
	_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);

	//unbind SRVs
	ID3D11ShaderResourceView* nullSRV[3] = { NULL, NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 3, nullSRV);
	//unbind UAVs
	ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	// Unbind Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);

}

void FluidGPU::ComputeImpulse(ID3D11DeviceContext* _deviceContext, ID3D11UnorderedAccessView* _targetUAV, ID3D11ShaderResourceView* _targetSRV, float _amount)
{
#pragma region Update Impulse Buffer
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ImpulseBuffer* dataPtr;

	result = _deviceContext->Map(m_DensityBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (ImpulseBuffer*)mappedResource.pData;
	dataPtr->amount = _amount;
	dataPtr->dt = m_timeStep;
	float iSize = FluidSize + (FluidSize * 2) + FluidSize;
	dataPtr->radius = m_GPUFluidVars.m_impulseRadius*iSize;
	dataPtr->sourcePos = XMFLOAT3((0.5f*FluidSize), (0.1f*(FluidSize * 2)), (0.5f*FluidSize));
	_deviceContext->Unmap(m_DensityBuffer, 0);
#pragma endregion

	//Target Impulse Compute shader
	_deviceContext->CSSetShader(m_ImpulseCS, nullptr, 0);

	//Give the shader the Impulse buffer
	_deviceContext->CSSetConstantBuffers(0, 1, &m_DensityBuffer);

	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &_targetUAV, 0);
	//Bind SRV
	_deviceContext->CSSetShaderResources(0, 1, &_targetSRV);

	//Dispatch Shader to GPU
	_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);

	//unbind SRV
	ID3D11ShaderResourceView* nullSRV[1] = { NULL };
	_deviceContext->CSSetShaderResources(0, 1, nullSRV);
	//unbind UAV
	ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	// Unbind Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidGPU::ComputeVorticity(ID3D11DeviceContext* _deviceContext)
{
	//Target the vorticity shader
	_deviceContext->CSSetShader(m_VorticityCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_VorticityUAV, 0);
	//Bind SRV
	_deviceContext->CSSetShaderResources(0, 1, &m_VelocitySRV[READ]);
	//Dispatch shader to th GPU
	_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);
	//Unbind SRVs
	ID3D11ShaderResourceView* nullSRV[1] = { NULL };
	_deviceContext->CSSetShaderResources(0, 1, nullSRV);
	//Unbind UAVs
	ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	//Unbind Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidGPU::ComputeConfinement(ID3D11DeviceContext* _deviceContext)
{
	//update constant buffer
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConfinementBuffer* dataPtr;
	result = _deviceContext->Map(m_ConfinementBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (ConfinementBuffer*)mappedResource.pData;
	dataPtr->dt = m_timeStep;
	dataPtr->VorticityStrength = m_GPUFluidVars.m_VorticityStrength;
	_deviceContext->Unmap(m_ConfinementBuffer, 0);

	//set the Compute shader
	_deviceContext->CSSetShader(m_ConfinementCS, nullptr, 0);
	//set constant buffer
	_deviceContext->CSSetConstantBuffers(0, 1, &m_ConfinementBuffer);
	//bind UAV
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_VelocityUAV[WRITE], 0);
	//bind SRV
	ID3D11ShaderResourceView *const SRV[3] = { m_BoundaryConditionsSRV, m_VorticitySRV, m_VelocitySRV[READ] };
	_deviceContext->CSSetShaderResources(0, 3, SRV);


	//Dispatch to GPU
	_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);

	//unbind SRV
	ID3D11ShaderResourceView* nullSRV[3] = { NULL, NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 3, nullSRV);
	//Unbind UAV
	ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidGPU::ComputeDivergence(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_DivergenceCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_DivergenceUAV, 0);
	ID3D11ShaderResourceView *const SRV[2] = { m_VelocitySRV[READ], m_BoundaryConditionsSRV };
	_deviceContext->CSSetShaderResources(0, 2, SRV);
	_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);

	ID3D11ShaderResourceView* nullSRV[2] = { NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 2, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[1] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidGPU::ComputeJacobi(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_JacobiCS, nullptr, 0);
	//bind the UAV to the shader 

	for (int i = 0; i < 10; i++)
	{
		//Set UAV
		_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_PressureUAV[WRITE], 0);
		//Set SRVs
		ID3D11ShaderResourceView *const SRV[3] = { m_PressureSRV[READ], m_BoundaryConditionsSRV, m_DivergenceSRV };
		_deviceContext->CSSetShaderResources(0, 3, SRV);
		//Dispatch Shader to the GPU
		_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);

		swap(m_PressureUAV[READ], m_PressureUAV[WRITE]);
		swap(m_PressureSRV[READ], m_PressureSRV[WRITE]);
	}
	//unbind SRVs
	ID3D11ShaderResourceView* nullSRV[3] = { NULL, NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 3, nullSRV);
	//unbind UAVs
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidGPU::ComputeProjection(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_ProjectionCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_VelocityUAV[WRITE], 0);
	//bind SRV
	ID3D11ShaderResourceView *const SRV[3] = { m_PressureSRV[READ], m_VelocitySRV[READ], m_BoundaryConditionsSRV };
	_deviceContext->CSSetShaderResources(0, 3, SRV);

	_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);

	ID3D11ShaderResourceView* nullSRV[3] = { NULL, NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 2, nullSRV);
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidGPU::Clear(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_ClearCS, nullptr, 0);
	//bind the UAV to the shader 
	ID3D11UnorderedAccessView *const UAVS[11] = { m_VelocityUAV[READ], m_VelocityUAV[WRITE], m_DensityUAV[READ], m_DensityUAV[WRITE], m_VorticityUAV, m_TemperatureUAV[READ], m_TemperatureUAV[WRITE], m_DivergenceUAV, m_PressureUAV[READ], m_PressureUAV[WRITE], m_TempUAV[READ]};
	_deviceContext->CSSetUnorderedAccessViews(0, 11, UAVS, 0);
	//dispatch
	_deviceContext->Dispatch((UINT)FluidSize / NUM_THREADS, (UINT)(FluidSize * 2) / NUM_THREADS, (UINT)FluidSize / NUM_THREADS);
}