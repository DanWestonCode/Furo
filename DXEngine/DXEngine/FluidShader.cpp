#include "FluidShader.h"
#define READ 0
#define WRITE 1
#define NUM_THREADS 8

FluidShader::FluidShader()
{
	m_ImpulseCS = nullptr;
	for (int i = 0; i < 2; i++)
	{
		m_DensitySRV[i] = nullptr;
		m_DensityUAV[i] = nullptr;
		m_Density[i] = nullptr;
	}
	
}
FluidShader::FluidShader(const FluidShader& other){}
FluidShader::~FluidShader(){}

HRESULT FluidShader::Initialize(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext, int _size)
{
	size = _size;

	CompileShaders(_device);
	CreateTextures(_device);

	HRESULT result;

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

	impulseRadius = 5.0f;
	densityAmount = 1.0f;
	m_decay = 0.0f;
	m_dissipation = 1.0f;
	m_ambientTemperature = 0.99f;
	m_buoyancy = 1.0f;
	m_weight = 0.0125f;

	ComputeBoundaryConditions(_deviceContext);

	return S_OK;
}

void FluidShader::CompileShaders(ID3D11Device* _device)
{
	HRESULT result;
	ID3DBlob* blob = nullptr;

	#pragma region Boundary Conditions
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/ComputeBoundaryConditions.hlsl", "ComputeBoundaryConditions", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_BoundaryConditionsCS);
	#pragma endregion
	
	#pragma region Advection
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/ComputeAdvection.hlsl", "ComputeAdvection", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_AdvectionCS);
	#pragma endregion

	#pragma region Buoyancy
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/ComputeBuoyancy.hlsl", "ComputeBuoyancy", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_BuoyancyCS);
	#pragma endregion

	#pragma region Impulse
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/Impulse.hlsl", "ComputeImpulse", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_ImpulseCS);
	#pragma endregion

	#pragma region Vorticity Confinement
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/ComputeVorticityConfinement.hlsl", "ComputeVorticityConfinement", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_VortConCS);
	#pragma endregion

	#pragma region Divergence
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/ComputeDivergence.hlsl", "ComputeDivergence", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_DivergenceCS);
	#pragma endregion

	#pragma region Jacobi
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/ComputeJacobi.hlsl", "ComputeJacobi", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_JacobiCS);
	#pragma endregion

	blob->Release();
}

void FluidShader::CreateTextures(ID3D11Device* _device)
{
	HRESULT result;
	D3D11_BUFFER_DESC outputDesc, inputDesc, sysOutputDesc;
	D3D11_SUBRESOURCE_DATA vinitData;

	#pragma region Texture3DDesc
	D3D11_TEXTURE3D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE3D_DESC));
	textureDesc.Width = size;
	textureDesc.Height = size;
	textureDesc.Depth = size;
	textureDesc.MipLevels = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	#pragma endregion

	#pragma region SRVDesc
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MostDetailedMip = 0;
	srvDesc.Texture3D.MipLevels = 1;
	#pragma endregion

	#pragma region UAVDesc
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	uavDesc.Texture3D.FirstWSlice = 0;
	uavDesc.Texture3D.MipSlice = 0;
	uavDesc.Texture3D.WSize = size;
	#pragma endregion

	#pragma region Boundary Conditions
	textureDesc.Format = DXGI_FORMAT_R8_SINT;
	srvDesc.Format = DXGI_FORMAT_R8_SINT;
	uavDesc.Format = DXGI_FORMAT_R8_SINT;

	//Create Texture3D
	result = (_device->CreateTexture3D(&textureDesc, NULL, &m_BoundaryConditions));
	//Create SRV
	result = (_device->CreateShaderResourceView(m_BoundaryConditions, NULL, &m_BoundaryConditionsSRV));
	//Create UAV
	result = (_device->CreateUnorderedAccessView(m_BoundaryConditions, NULL, &m_BoundaryConditionsUAV));
	#pragma endregion

	#pragma region Divergence
	textureDesc.Format = DXGI_FORMAT_R16_FLOAT;
	srvDesc.Format = DXGI_FORMAT_R16_FLOAT;
	uavDesc.Format = DXGI_FORMAT_R16_FLOAT;

	result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Divergence));
	//Create SRV
	result = (_device->CreateShaderResourceView(m_Divergence, &srvDesc, &m_DivergenceSRV));
	//Create UAV
	result = (_device->CreateUnorderedAccessView(m_Divergence, &uavDesc, &m_DivergenceUAV));	
	#pragma endregion

	for (int i = 0; i < 2; i++)
	{
		#pragma region Pressure
		textureDesc.Format = DXGI_FORMAT_R16_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R16_FLOAT;
		uavDesc.Format = DXGI_FORMAT_R16_FLOAT;
		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Pressure[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_Pressure[i], NULL, &m_PressureSRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_Pressure[i], NULL, &m_PressureUAV[i]));
		#pragma endregion

		#pragma region Density
		textureDesc.Format = DXGI_FORMAT_R16_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R16_FLOAT;
		uavDesc.Format = DXGI_FORMAT_R16_FLOAT;
		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Density[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_Density[i], &srvDesc, &m_DensitySRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_Density[i], &uavDesc, &m_DensityUAV[i]));
		#pragma endregion

		#pragma region Velocity
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Velocity[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_Velocity[i], &srvDesc, &m_VelocitySRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_Velocity[i], &uavDesc, &m_VelocityUAV[i]));
		#pragma endregion

		#pragma region Temperature
		textureDesc.Format = DXGI_FORMAT_R16_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R16_FLOAT;
		uavDesc.Format = DXGI_FORMAT_R16_FLOAT;

		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Temperature[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_Temperature[i], &srvDesc, &m_TemperatureSRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_Temperature[i], &uavDesc, &m_TemperatureUAV[i]));
		#pragma endregion

		#pragma region Vorticity
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

		//Create Texture3D
		result = (_device->CreateTexture3D(&textureDesc, NULL, &m_Vorticity[i]));
		//Create SRV
		result = (_device->CreateShaderResourceView(m_Vorticity[i], &srvDesc, &m_VorticitySRV[i]));
		//Create UAV
		result = (_device->CreateUnorderedAccessView(m_Vorticity[i], &uavDesc, &m_VorticityUAV[i]));
		#pragma endregion
	}	
}

void FluidShader::Shutdown()
{
	m_ImpulseCS->Release();
	delete m_ImpulseCS;
	for (int i = 0; i < 2; i++)
	{
		m_Density[i] = nullptr;
		m_DensityUAV[i] = nullptr;
		m_DensitySRV[i] = nullptr;
	}

	delete m_Density;
	delete m_DensityUAV;
	delete m_DensitySRV;

	ShaderBase::Shutdown();
}

void FluidShader::Update(ID3D11DeviceContext* _deviceContext, float _dt)
{
	m_timeStep = _dt;

	ComputeAdvection(_deviceContext, m_TemperatureUAV[WRITE], m_TemperatureSRV[READ]);
	std::swap(m_TemperatureUAV[READ], m_TemperatureUAV[WRITE]);
	std::swap(m_TemperatureSRV[READ], m_TemperatureSRV[WRITE]);

	ComputeAdvection(_deviceContext, m_DensityUAV[WRITE], m_DensitySRV[READ]);
	std::swap(m_DensityUAV[READ], m_DensityUAV[WRITE]);
	std::swap(m_DensitySRV[READ], m_DensitySRV[WRITE]);

	ComputeAdvection(_deviceContext, m_VelocityUAV[WRITE], m_VelocitySRV[READ]);
	std::swap(m_VelocityUAV[READ], m_VelocityUAV[WRITE]);
	std::swap(m_VelocitySRV[READ], m_VelocitySRV[WRITE]);

	ComputeBuoyancy(_deviceContext);
	std::swap(m_VelocityUAV[READ], m_VelocityUAV[WRITE]);
	std::swap(m_VelocitySRV[READ], m_VelocitySRV[WRITE]);

	ComputeImpulse(_deviceContext, m_DensityUAV[WRITE], m_DensitySRV[READ]);
	std::swap(m_DensityUAV[READ], m_DensityUAV[WRITE]);
	std::swap(m_DensitySRV[READ], m_DensitySRV[WRITE]);

	ComputeImpulse(_deviceContext, m_TemperatureUAV[WRITE], m_TemperatureSRV[READ]);
	std::swap(m_TemperatureUAV[READ], m_TemperatureUAV[WRITE]);
	std::swap(m_TemperatureSRV[READ], m_TemperatureSRV[WRITE]);

	ComputeVorticityConfinement(_deviceContext);
	std::swap(m_VelocityUAV[READ], m_VelocityUAV[WRITE]);
	std::swap(m_VelocitySRV[READ], m_VelocitySRV[WRITE]);

	ComputeDivergence(_deviceContext);

	ComputeJacobi(_deviceContext);
}

void FluidShader::ComputeBoundaryConditions(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_BoundaryConditionsCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_BoundaryConditionsUAV, 0);
	//Dispatch Shader
	_deviceContext->Dispatch((UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS);

	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidShader::ComputeAdvection(ID3D11DeviceContext* _deviceContext, ID3D11UnorderedAccessView* _TargetWrite, ID3D11ShaderResourceView* _TargetRead)
{
	#pragma region Update Advection Buffer
	AdvectionBuffer* dataPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = _deviceContext->Map(m_AdvectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (AdvectionBuffer*)mappedResource.pData;
	dataPtr->dissipation = m_dissipation;
	dataPtr->decay = m_decay;
	dataPtr->dt = m_timeStep;
	_deviceContext->Unmap(m_AdvectionBuffer, 0);
	#pragma endregion 

	//target advection shader
	_deviceContext->CSSetShader(m_AdvectionCS, nullptr, 0);

	_deviceContext->CSSetConstantBuffers(0, 1, &m_AdvectionBuffer);

	//set sampler in Compute shader
	_deviceContext->CSSetSamplers(0, 1, &m_Sampler);

	//bind the target as the UAV for the advection shader
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &_TargetWrite, 0);

	//Set the field to advect against
	ID3D11ShaderResourceView *const SRV[3] = { _TargetRead, m_BoundaryConditionsSRV, m_VelocitySRV[READ] };
	_deviceContext->CSSetShaderResources(0, 3, SRV);

	_deviceContext->Dispatch((UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS);

	ID3D11ShaderResourceView* nullSRV[3] = { NULL, NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 3, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidShader::ComputeBuoyancy(ID3D11DeviceContext* _deviceContext)
{
	#pragma region Update Buoyancy Buffer
	BuoyancyBuffer* dataPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = _deviceContext->Map(m_BuoyancyBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (BuoyancyBuffer*)mappedResource.pData;
	dataPtr->ambientTemperature = m_ambientTemperature;
	dataPtr->buoyancy = m_buoyancy;
	dataPtr->dt = m_timeStep;
	dataPtr->weight = m_weight;
	_deviceContext->Unmap(m_BuoyancyBuffer, 0);
	#pragma endregion 

	//target advection shader
	_deviceContext->CSSetShader(m_BuoyancyCS, nullptr, 0);

	_deviceContext->CSSetConstantBuffers(0, 1, &m_BuoyancyBuffer);


	//bind the target as the UAV for the advection shader
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_VelocityUAV[WRITE], 0);

	//Set the field to advect against
	ID3D11ShaderResourceView *const SRV[3] = { m_VelocitySRV[READ], m_DensitySRV[READ], m_VelocitySRV[READ] };
	_deviceContext->CSSetShaderResources(0, 3, SRV);

	_deviceContext->Dispatch((UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS);

	ID3D11ShaderResourceView* nullSRV[3] = { NULL, NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 3, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);

}

void FluidShader::ComputeImpulse(ID3D11DeviceContext* _deviceContext, ID3D11UnorderedAccessView* _targetUAV, ID3D11ShaderResourceView* _targetSRV)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ImpulseBuffer* dataPtr;

	result = _deviceContext->Map(m_DensityBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (ImpulseBuffer*)mappedResource.pData;
	dataPtr->amount = densityAmount;
	dataPtr->dt = m_timeStep;
	dataPtr->radius = impulseRadius;
	dataPtr->sourcePos = XMFLOAT3(16.0f, 1.0f, 16.0f);
	_deviceContext->Unmap(m_DensityBuffer, 0);

	_deviceContext->CSSetConstantBuffers(0, 1, &m_DensityBuffer);

	//set the Compute shader
	_deviceContext->CSSetShader(m_ImpulseCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &_targetUAV, 0);
	_deviceContext->CSSetShaderResources(0, 1, &_targetSRV);
	_deviceContext->Dispatch((UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS);

	ID3D11ShaderResourceView* nullSRV[1] = { NULL };
	_deviceContext->CSSetShaderResources(0, 1, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidShader::ComputeVorticityConfinement(ID3D11DeviceContext* _deviceContext)
{
	HRESULT result;

	//set the Compute shader
	_deviceContext->CSSetShader(m_VortConCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_VelocityUAV[WRITE], 0);
	ID3D11ShaderResourceView *const SRV[2] = { m_VelocitySRV[READ], m_BoundaryConditionsSRV };
	_deviceContext->CSSetShaderResources(0, 2, SRV);
	_deviceContext->Dispatch((UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS);

	ID3D11ShaderResourceView* nullSRV[2] = { NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 2, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);

}

void FluidShader::ComputeDivergence(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_DivergenceCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_DivergenceUAV, 0);
	ID3D11ShaderResourceView *const SRV[2] = { m_VelocitySRV[READ], m_BoundaryConditionsSRV };
	_deviceContext->CSSetShaderResources(0, 2, SRV);
	_deviceContext->Dispatch((UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS);

	ID3D11ShaderResourceView* nullSRV[2] = { NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 2, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}

void FluidShader::ComputeJacobi(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_JacobiCS, nullptr, 0);
	//bind the UAV to the shader 

	for (int i = 0; i < 10; i ++)
	{
		_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_PressureUAV[WRITE], 0);
		ID3D11ShaderResourceView *const SRV[3] = { m_PressureSRV[READ], m_BoundaryConditionsSRV, m_DivergenceSRV };
		_deviceContext->CSSetShaderResources(0, 3, SRV);
		_deviceContext->Dispatch((UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS, (UINT)size / NUM_THREADS);

		std::swap(m_PressureUAV[READ], m_PressureUAV[WRITE]);
		std::swap(m_PressureSRV[READ], m_PressureSRV[WRITE]);
	}
	
	ID3D11ShaderResourceView* nullSRV[3] = { NULL, NULL, NULL };
	_deviceContext->CSSetShaderResources(0, 3, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}