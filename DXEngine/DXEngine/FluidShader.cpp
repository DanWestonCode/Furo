#include "FluidShader.h"
#define READ 0
#define WRITE 1
#define NUMTHREADS 8

FluidShader::FluidShader()
{
	m_BoundaryConditionsCS = nullptr;
	m_BoundaryConditions = nullptr;

	m_Velocity[READ] = nullptr;
	m_Velocity[WRITE] = nullptr;
	m_Density[READ] = nullptr;
	m_Density[WRITE] = nullptr;
	m_Temperature[READ] = nullptr;
	m_Temperature[WRITE] = nullptr;
	m_Vorticity[READ] = nullptr;
	m_Vorticity[WRITE] = nullptr;

	m_ImpulseBuffer = nullptr;
	m_AdvectionBuffer = nullptr;
	m_FluidBuffer = nullptr;
}
FluidShader::FluidShader(const FluidShader& other){}
FluidShader::~FluidShader(){}

HRESULT FluidShader::Initialize(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{
	CompileShaders(_device);
	CreateTextures(_device);
	CreateSampler(_device);

	HRESULT result;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	//fluid buffer
	bufferDesc.ByteWidth = sizeof(FluidBuffer);
	_device->CreateBuffer(&bufferDesc, NULL, &m_FluidBuffer);

	//advection buffer
	bufferDesc.ByteWidth = sizeof(AdvectionBuffer);
	_device->CreateBuffer(&bufferDesc, NULL, &m_AdvectionBuffer);

	//Impulse buffer
	bufferDesc.ByteWidth = sizeof(ImpulseBuffer);
	_device->CreateBuffer(&bufferDesc, NULL, &m_ImpulseBuffer);

	ComputeBoundaryConditions(_deviceContext);
	return S_OK;
}

void FluidShader::CompileShaders(ID3D11Device* _device)
{
	HRESULT result;
	ID3DBlob* blob = nullptr;

	#pragma region Boundary Conditions Compute
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/BoundaryConditions.hlsl", "ComputeBoundaryConditions", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_BoundaryConditionsCS);
	#pragma endregion

	#pragma region Advection Compute
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/Advection.hlsl", "ComputeAdvection", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_AdvectionCS);
	#pragma endregion

	blob->Release();
}

void FluidShader::CreateTextures(ID3D11Device* _device)
{
	HRESULT result;
	D3D11_BUFFER_DESC outputDesc, inputDesc, sysOutputDesc;
	D3D11_SUBRESOURCE_DATA vinitData;
	int numElements = 32;

	#pragma region Texture3DDesc
	D3D11_TEXTURE3D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE3D_DESC));
	textureDesc.Width = 32;
	textureDesc.Height = 32;
	textureDesc.Depth = 32;
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
	uavDesc.Texture3D.WSize = 32;
	#pragma endregion

	#pragma region Boundary Conditions
	textureDesc.Format = DXGI_FORMAT_R8_SINT;
	srvDesc.Format = DXGI_FORMAT_R8_SINT;
	uavDesc.Format = DXGI_FORMAT_R8_SINT;

	//Create Texture3D
	result = (_device->CreateTexture3D(&textureDesc, NULL, &m_BoundaryConditions));
	//Create SRV
	result = (_device->CreateShaderResourceView(m_BoundaryConditions, &srvDesc, &m_BoundaryConditionsSRV));
	//Create UAV
	result = (_device->CreateUnorderedAccessView(m_BoundaryConditions, &uavDesc, &m_BoundaryConditionsUAV));
	#pragma endregion

	for (int i = 0; i < 2; i++)
	{
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

void FluidShader::CreateSampler(ID3D11Device* _device)
{
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
	HRESULT hresult = _device->CreateSamplerState(&samplerDesc, &m_Sampler);
}

void FluidShader::Shutdown()
{
	m_BoundaryConditionsCS->Release();
	delete m_BoundaryConditionsCS;
	m_BoundaryConditions = nullptr;
	delete m_BoundaryConditions;
	m_BoundaryConditionsSRV = nullptr;
	delete m_BoundaryConditionsSRV;
	m_BoundaryConditionsUAV = nullptr;
	delete m_BoundaryConditionsUAV;

	ShaderBase::Shutdown();
}

void FluidShader::Update(ID3D11DeviceContext* _deviceContext, float _dt)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	FluidBuffer* dataPtr;
	//update General buffer
	HRESULT result = _deviceContext->Map(m_AdvectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (FluidBuffer*)mappedResource.pData;
	dataPtr->dt = _dt;
	dataPtr->fDensityBuoyancy = 0;
	dataPtr->fDensityWeight = 0;
	dataPtr->fVorticityStrength = 0;
	dataPtr->vBuoyancyDirection = XMFLOAT3(0,1,0);
	_deviceContext->Unmap(m_AdvectionBuffer, 0);


	//advect temp against velocity
	ComputeAdvection(_deviceContext, m_TemperatureUAV[WRITE], m_TemperatureSRV[READ], m_VelocitySRV[READ], 1.0f, 1.0f);
	std::swap(m_TemperatureUAV[READ], m_TemperatureUAV[WRITE]);
	std::swap(m_TemperatureSRV[READ], m_TemperatureSRV[WRITE]);
	//std::swap(m_Temperature[READ], m_Temperature[WRITE]);

	//advect density against velocity
	ComputeAdvection(_deviceContext, m_DensityUAV[WRITE], m_DensitySRV[READ], m_VelocitySRV[READ], 1.0f, 1.0f);
	std::swap(m_TemperatureUAV[READ], m_TemperatureUAV[WRITE]);
	std::swap(m_TemperatureSRV[READ], m_TemperatureSRV[WRITE]);

	//advect velocity against velocity
	ComputeAdvection(_deviceContext, m_VelocityUAV[WRITE], m_VelocitySRV[READ], m_VelocitySRV[READ], 1.0f, 1.0f);
	std::swap(m_TemperatureUAV[READ], m_TemperatureUAV[WRITE]);
	std::swap(m_TemperatureSRV[READ], m_TemperatureSRV[WRITE]);
}

void FluidShader::ComputeBoundaryConditions(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_BoundaryConditionsCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_BoundaryConditionsUAV, 0);
	//Dispatch Shader
	_deviceContext->Dispatch((UINT)m_volSize / NUMTHREADS, (UINT)m_volSize / NUMTHREADS, (UINT)m_volSize / NUMTHREADS);
	//Unbind
	ComputeUnbind(_deviceContext, 1, 0);
}

void FluidShader::ComputeAdvection(ID3D11DeviceContext* _deviceContext, ID3D11UnorderedAccessView* _targetUAV, ID3D11ShaderResourceView* _targetSRV, ID3D11ShaderResourceView* _velocitySRV, float _dissipation, float _decay)
{

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	AdvectionBuffer* dataPtr;
	// Lock the screen size constant buffer so it can be written to.
	HRESULT result = _deviceContext->Map(m_AdvectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (AdvectionBuffer*)mappedResource.pData;
	dataPtr->dissapation = _dissipation;
	dataPtr->dtMod = 0.0f;
	dataPtr->decay = _decay;
	_deviceContext->Unmap(m_AdvectionBuffer, 0);

	ID3D11Buffer *const pProcessConstantBuffers[2] = { m_FluidBuffer, m_AdvectionBuffer};
	_deviceContext->CSSetConstantBuffers(0, 2, pProcessConstantBuffers);

	//set sampler in Compute shader
	_deviceContext->CSSetSamplers(0, 1, &m_Sampler);
	//target advection shader
	_deviceContext->CSSetShader(m_AdvectionCS, nullptr, 0);
	//bind the target as the UAV for the advection shader
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &_targetUAV, 0);
	//Set the field to advect against
	ID3D11ShaderResourceView *const SRV[3] = { _targetSRV, m_BoundaryConditionsSRV, _velocitySRV };
	_deviceContext->CSSetShaderResources(0, 3, SRV);
	//Dispatch Shader
	_deviceContext->Dispatch((UINT)m_volSize / NUMTHREADS, (UINT)m_volSize / NUMTHREADS, (UINT)m_volSize / NUMTHREADS);
	ComputeUnbind(_deviceContext, 1, 3);
}

void FluidShader::ComputeUnbind(ID3D11DeviceContext* _deviceContext, int _numUAVS, int _numSRVS)
{
	if (_numUAVS > 0)
	{
		// Unbind output from compute shader
		ID3D11UnorderedAccessView* nullUAV[] = { NULL };
		for (int i = 1; i <= _numUAVS; i++)
		{
			_deviceContext->CSSetUnorderedAccessViews(0, i, nullUAV, 0);
		}
	}

	if (_numSRVS)
	{
		// Unbind output from compute shader
		ID3D11ShaderResourceView* nullSRV[] = { NULL };
		for (int i = 1; i <= _numUAVS; i++)
		{
			_deviceContext->CSSetShaderResources(0, i, nullSRV);
		}
	}

	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}