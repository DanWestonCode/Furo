#include "AABBVolumeRayCastShader.h"

AABBVolumeRaycastShader::AABBVolumeRaycastShader()
{
}
AABBVolumeRaycastShader::AABBVolumeRaycastShader(const AABBVolumeRaycastShader& other){}
AABBVolumeRaycastShader::~AABBVolumeRaycastShader(){};

HRESULT AABBVolumeRaycastShader::Initialize(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext, HWND _hwnd)
{
	HRESULT result;
	ID3DBlob* blob = nullptr;

#pragma region Vertex Shader
	result = CompileShaderFromFile(L"../DXEngine/AABBRayCast.hlsl", "RayCastVS", "vs_5_0", &blob);
	result = _device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_VertexShader);
#pragma endregion	

#pragma region Input Layout
	// Shader Input Layout
	D3D11_INPUT_ELEMENT_DESC m_layout[1];
	m_layout[0].SemanticName = "POSITION"; //match name in VS shader
	m_layout[0].SemanticIndex = 0;
	m_layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	m_layout[0].InputSlot = 0;
	m_layout[0].AlignedByteOffset = 0;
	m_layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	m_layout[0].InstanceDataStepRate = 0;

	UINT numElements = ARRAYSIZE(m_layout);

	result = _device->CreateInputLayout(m_layout, numElements, blob->GetBufferPointer(), blob->GetBufferSize(), &m_InputLayout);
	blob->Release();
#pragma endregion	

#pragma region Pixel Shader
	blob = nullptr;
	// Compile and create the pixel shader
	result = CompileShaderFromFile(L"../DXEngine/raycast.hlsl", "RayCastPS", "ps_5_0", &blob);
	result = _device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_PixelShader);
	blob->Release();
#pragma  endregion	

	return result;
}

void AABBVolumeRaycastShader::InitilializeBuffers(ID3D11Device*_device, ID3D11DeviceContext*_deviceContext, Cube* _cube)
{
	HRESULT result;

#pragma region Constant Buffers
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	//camera buffer
	bufferDesc.ByteWidth = sizeof(CamBuffer);
	result = _device->CreateBuffer(&bufferDesc, NULL, &m_CamBuffer);

	//object buffer 
	bufferDesc.ByteWidth = sizeof(ObjectBuffer);
	result = _device->CreateBuffer(&bufferDesc, NULL, &m_ObjectBuffer);

	//Fluid buffer 
	bufferDesc.ByteWidth = sizeof(FluidBuffer);
	result = _device->CreateBuffer(&bufferDesc, NULL, &m_FluidBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CamBuffer* camPtr;
	ObjectBuffer* objPtr;
	FluidBuffer* fluidPtr;

	//Cam Buffer
	result = _deviceContext->Map(m_CamBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CamBuffer*)mappedResource.pData;
	camPtr->CameraPos = Camera::Instance()->m_pos;
	_deviceContext->Unmap(m_CamBuffer, 0);

	//Object Buffer
	result = _deviceContext->Map(m_ObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	objPtr = (ObjectBuffer*)mappedResource.pData;
	objPtr->ObjectPos = _cube->m_pos;
	objPtr->ObjectScale = _cube->m_scale;
	_deviceContext->Unmap(m_ObjectBuffer, 0);

	//fluid buffer
	result = _deviceContext->Map(m_FluidBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	fluidPtr = (FluidBuffer*)mappedResource.pData;
	fluidPtr->Absoprtion = 60;
	fluidPtr->Samples = 64;
	_deviceContext->Unmap(m_FluidBuffer, 0);
#pragma endregion

}

void AABBVolumeRaycastShader::Shutdown()
{
	m_CamBuffer->Release();
	delete m_CamBuffer;
	m_ObjectBuffer->Release();
	delete m_ObjectBuffer;
	m_FluidBuffer->Release();
	delete m_FluidBuffer;

	ShaderBase::Shutdown();
}