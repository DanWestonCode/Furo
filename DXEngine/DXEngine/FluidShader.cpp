#include "FluidShader.h"

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
	CreateBuffers(_device);
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
	blob->Release();
	#pragma endregion
}

void FluidShader::CreateBuffers(ID3D11Device* _device)
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
	textureDesc.Format = DXGI_FORMAT_R16_FLOAT;
	srvDesc.Format = DXGI_FORMAT_R16_FLOAT;
	uavDesc.Format = DXGI_FORMAT_R16_FLOAT;

	//Create Texture3D
	result = (_device->CreateTexture3D(&textureDesc, NULL, &m_BoundaryConditions));
	//Create SRV
	result = (_device->CreateShaderResourceView(m_BoundaryConditions, &srvDesc, &m_BoundaryConditionsSRV));
	//Create UAV
	result = (_device->CreateUnorderedAccessView(m_BoundaryConditions, &uavDesc, &m_BoundaryConditionsUAV));
	#pragma endregion

}

void FluidShader::Shutdown()
{
	m_BoundaryConditionsCS->Release();
	delete m_BoundaryConditionsCS;
	m_BoundaryConditions = nullptr;
	delete m_BoundaryConditions;

	ShaderBase::Shutdown();
}

void FluidShader::Render(ID3D11DeviceContext* _deviceContext)
{
	HRESULT result;

	//set the Compute shader
	_deviceContext->CSSetShader(m_BoundaryConditionsCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_BoundaryConditionsUAV, 0);
	_deviceContext->Dispatch(8, 8, 1);

	// Unbind output from compute shader
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);

	result = S_OK;
}