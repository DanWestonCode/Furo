#include "FluidShader.h"

FluidShader::FluidShader()
{
	m_BoundaryConditionsCS = nullptr;
	m_outputBuffer = nullptr;
	m_BoundaryConditionsWRITE = nullptr;
	m_BoundaryConditionsREAD = nullptr;
	m_outputresult = nullptr;
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
	int numElements = 16;

	#pragma region Boundary Conditions
	m_BoundaryConditionsWRITE = new BoundaryConditions[numElements];
	m_BoundaryConditionsREAD = new BoundaryConditions[numElements];
	std::memset(m_BoundaryConditionsWRITE, 0, sizeof(BoundaryConditions)*numElements);
	std::memset(m_BoundaryConditionsREAD, 0, sizeof(BoundaryConditions)*numElements);


	//Output buffer to be bound to UAV
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.ByteWidth = sizeof(BoundaryConditions) * numElements;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = sizeof(BoundaryConditions);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	
	result =_device->CreateBuffer(&outputDesc, 0, &m_outputBuffer);

	//System buffer to get GPU result
	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	result = _device->CreateBuffer(&outputDesc, 0, &m_outputresult);

	//UAV to send to shader 
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = numElements;
	result =_device->CreateUnorderedAccessView(m_outputBuffer, &uavDesc, &m_BoundaryConditionsUAV);
	#pragma endregion

}

void FluidShader::Shutdown()
{
	m_BoundaryConditionsCS->Release();
	delete m_BoundaryConditionsCS;
	m_outputBuffer->Release();
	delete m_outputBuffer;
	m_BoundaryConditionsWRITE = nullptr;
	delete m_BoundaryConditionsWRITE;
	m_outputresult = nullptr;
	delete m_outputresult;

	ShaderBase::Shutdown();
}

void FluidShader::Render(ID3D11DeviceContext* _deviceContext)
{
	HRESULT result;

	//set the Compute shader
	_deviceContext->CSSetShader(m_BoundaryConditionsCS, nullptr, 0);
	//bind the UAV to the shader 
	_deviceContext->CSSetUnorderedAccessViews(0, 1, &m_BoundaryConditionsUAV, 0);
	_deviceContext->Dispatch(1, 1, 1);

	// Unbind output from compute shader
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	_deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	// Disable Compute Shader
	_deviceContext->CSSetShader(nullptr, nullptr, 0);
	
	_deviceContext->CopyResource(m_outputresult, m_outputBuffer);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	result = _deviceContext->Map(m_outputresult, 0, D3D11_MAP_READ, 0, &mappedData);

	m_BoundaryConditionsREAD = reinterpret_cast<BoundaryConditions*>(mappedData.pData);
	
	for (int i = 0; i < 16; i++)
	{
		Debug::Instance()->Log(m_BoundaryConditionsREAD[i].x.x);
	}

	_deviceContext->Unmap(m_outputresult, 0);

	result = S_OK;
}