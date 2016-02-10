#include "FluidShader.h"
#define READ 0
#define WRITE 1

FluidShader::FluidShader()
{
	m_BoundaryConditionsCS = nullptr;
	m_BoundaryConditions = nullptr;
}
FluidShader::FluidShader(const FluidShader& other){}
FluidShader::~FluidShader(){}

HRESULT FluidShader::Initialize(ID3D11Device* _device)
{
	CompileShaders(_device);
	CreateTextures(_device);
	return S_OK;
}

void FluidShader::CompileShaders(ID3D11Device* _device)
{
	HRESULT result;
	ID3DBlob* blob = nullptr;

	#pragma region Boundary Conditions
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/fluid.hlsl", "ComputeBoundaryConditions", "cs_5_0", &blob);
	result = _device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_BoundaryConditionsCS);
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

void FluidShader::Render(ID3D11DeviceContext* _deviceContext)
{
	ComputeBoundaryConditions(_deviceContext);	
}

void FluidShader::ComputeBoundaryConditions(ID3D11DeviceContext* _deviceContext)
{
	//set the Compute shader
	_deviceContext->CSSetShader(m_BoundaryConditionsCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_BoundaryConditionsUAV, 0);
	//Dispatch Shader
	_deviceContext->Dispatch(2, 2, 2);
	//Unbind
	ComputeUnbind(_deviceContext, 1);
}

void FluidShader::ComputeUnbind(ID3D11DeviceContext* _deviceContext, int _numViews)
{
	// Unbind output from compute shader
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	for (int i = 1; i <= _numViews; i++)
	{
		_deviceContext->CSSetUnorderedAccessViews(0, i, nullUAV, 0);
	}
	
	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
}