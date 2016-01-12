#include "RayCaster.h"


RayCaster::RayCaster(){}
RayCaster::RayCaster(const RayCaster& other){}
RayCaster::~RayCaster(){}

bool RayCaster::Initialize(ID3D11Device* device, HWND hwnd)
{
	InitializeStandardShader(device, hwnd);
	InitializeRayCastShader(device, hwnd);
	CreateConstantBuffer(device);
	InitRenderTextures(device, 500, 500);
	InitSampler(device);
	InitRasterStates(device);
	InitVolumeTexture(device);
	InitRenderCube(device);


	return true;
}

void RayCaster::InitializeStandardShader(ID3D11Device* device, HWND hwnd)
{
	//Set up of the layout for the vertex buffer 
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	WCHAR* vsFilename = L"../DXFrameWork/color.fx";
	WCHAR* psFilename = L"../DXFrameWork/color.fx";

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code into buffers
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);

	//Create shader objects out of shaders
	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_StandardVS);

	D3D11_INPUT_ELEMENT_DESC layout[2];
	layout[0].SemanticName = "POSITION"; //match name in VS shader
	layout[0].SemanticIndex = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].AlignedByteOffset = 0;
	layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;

	layout[1].SemanticName = "COLOR";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	UINT numElements = ARRAYSIZE(layout);

	// Create the vertex input layout.
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_StandardLayout);

	// Compile the pixel shader code into buffers
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_StandardPS);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
}
void RayCaster::InitializeRayCastShader(ID3D11Device* device, HWND hwnd)
{
	//Set up of the layout for the vertex buffer 
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	WCHAR* vsFilename = L"../DXFrameWork/raycast.hlsl";
	WCHAR* psFilename = L"../DXFrameWork/raycast.hlsl";

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code into buffers
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "RayCastVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);

	//Create shader objects out of shaders
	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_RayCasterVS);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the vertex input layout.
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_RayCasterLayout);

	// Compile the pixel shader code into buffers
	result = D3DCompileFromFile(psFilename, NULL, NULL, "RayCastPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_RayCasterPS);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	
}
void RayCaster::CreateConstantBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC matrixBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
}
void RayCaster::InitRenderTextures(ID3D11Device* device, int width, int height)
{
	D3D11_TEXTURE2D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.ArraySize = 1;
	descTex.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descTex.Width = width;
	descTex.Height = height;
	descTex.MipLevels = 1;
	descTex.SampleDesc.Count = 1;
	descTex.CPUAccessFlags = 0;

	// Front and back side
	for (int i = 0; i < 2; i++)
	{
		(device->CreateTexture2D(&descTex, NULL, &m_Texture2D[i]));

		// Create resource view
		device->CreateShaderResourceView(m_Texture2D[i], NULL, &m_ShaderResource[i]);

		// Create render target view
		device->CreateRenderTargetView(m_Texture2D[i], NULL, &m_RenderTargetView[i]);
	}
}
void RayCaster::InitRasterStates(ID3D11Device* device)
{
	// Back face culling
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = true;
	(device->CreateRasterizerState(&rasterizerDesc, &backfacecull));

	// Front face culling
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthClipEnable = true;
	(device->CreateRasterizerState(&rasterizerDesc, &frontfacecull));

}
void RayCaster::InitVolumeTexture(ID3D11Device*device)
{
	const UINT g_iVolumeSize = 256;	// voxel volume width, height and depth
	const UINT g_vol = 178;
	HANDLE hFile = CreateFileW(L"../skull.raw", GENERIC_READ, 0, NULL, OPEN_EXISTING, OPEN_EXISTING, NULL);

	BYTE *buffer = (BYTE *)malloc(g_iVolumeSize*g_iVolumeSize*g_iVolumeSize * sizeof(BYTE));

	DWORD numberOfBytesRead = 0;

	CloseHandle(hFile);

	D3D11_TEXTURE3D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.Height = g_iVolumeSize;
	descTex.Width = g_iVolumeSize;
	descTex.Depth = g_iVolumeSize;
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R8_UNORM;
	descTex.Usage = D3D11_USAGE_IMMUTABLE;
	descTex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	descTex.CPUAccessFlags = 0;
	// Initial data
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = buffer;
	initData.SysMemPitch = g_iVolumeSize;
	initData.SysMemSlicePitch = g_iVolumeSize*g_iVolumeSize;
	// Create texture
	(device->CreateTexture3D(&descTex, &initData, &m_Texture3D));

	// Create a resource view of the texture
	(device->CreateShaderResourceView(m_Texture3D, NULL, &m_VolumeShaderRV));

	free(buffer);
}
void RayCaster::InitSampler(ID3D11Device* device)
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	(device->CreateSamplerState(&sampDesc, &m_SamplerState));
}
void RayCaster::InitRenderCube(ID3D11Device* device)
{
	HRESULT result;
	unsigned long* indices;

	m_VertexCount = 6 * 6;
	// Set the number of indices in the index array.
	m_IndexCount = m_VertexCount;

	// Create the vertex array.
	m_vertices = new VertexLayout[m_VertexCount];

	// Create the index array.
	indices = new unsigned long[m_IndexCount];

	//set up indices
	for (int i = 0; i < m_VertexCount; i++)
	{
		indices[i] = i;
	}

	float m_floatSize = 20;

	int vert = 0;
	Color colour = Color(1.0f, 0.0f, 0.0f, 1.0f);
	Color colour2 = Color(0.0f, 1.0f, 0.0f, 1.0f);
	Color colour3 = Color(0.0f, 0.0f, 1.0f, 1.0f);
	Color colour4 = Color(1.0f, 1.0f, 0.0f, 1.0f);
	Color colour5 = Color(0.0f, 1.0f, 1.0f, 1.0f);
	Color colour6 = Color(1.0f, 0.0f, 1.0f, 1.0f);

	//top
	m_vertices[vert].color = colour;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0 + m_floatSize);
	m_vertices[vert].color = colour;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0 + m_floatSize, 0 + m_floatSize);
	m_vertices[vert].color = colour;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0 + m_floatSize, 0);

	m_vertices[vert].color = colour;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0 + m_floatSize, 0);
	m_vertices[vert].color = colour;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0);
	m_vertices[vert].color = colour;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0 + m_floatSize);

	//front
	m_vertices[vert].color = colour2;
	m_vertices[vert++].position = Vector3(0, 0, 0 + m_floatSize);
	m_vertices[vert].color = colour2;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0 + m_floatSize);
	m_vertices[vert].color = colour2;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0 + m_floatSize);

	m_vertices[vert].color = colour2;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0 + m_floatSize);
	m_vertices[vert].color = colour2;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0 + m_floatSize, 0 + m_floatSize);
	m_vertices[vert].color = colour2;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0 + m_floatSize);

	//right
	m_vertices[vert].color = colour3;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0);
	m_vertices[vert].color = colour3;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0 + m_floatSize, 0);
	m_vertices[vert].color = colour3;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0 + m_floatSize, 0 + m_floatSize);

	m_vertices[vert].color = colour3;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0);
	m_vertices[vert].color = colour3;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0 + m_floatSize, 0 + m_floatSize);
	m_vertices[vert].color = colour3;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0 + m_floatSize);

	//down
	m_vertices[vert].color = colour4;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0);
	m_vertices[vert].color = colour4;
	m_vertices[vert++].position = Vector3(0, 0, 0 + m_floatSize);
	m_vertices[vert].color = colour4;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0 + m_floatSize);

	m_vertices[vert].color = colour4;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0);
	m_vertices[vert].color = colour4;
	m_vertices[vert++].position = Vector3(0, 0, 0);
	m_vertices[vert].color = colour4;
	m_vertices[vert++].position = Vector3(0, 0, 0 + m_floatSize);

	//back
	m_vertices[vert].color = colour5;
	m_vertices[vert++].position = Vector3(0, 0, 0);
	m_vertices[vert].color = colour5;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0);
	m_vertices[vert].color = colour5;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0);

	m_vertices[vert].color = colour5;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0);
	m_vertices[vert].color = colour5;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0 + m_floatSize, 0);
	m_vertices[vert].color = colour5;
	m_vertices[vert++].position = Vector3(0 + m_floatSize, 0, 0);

	//left
	m_vertices[vert].color = colour6;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0 + m_floatSize);
	m_vertices[vert].color = colour6;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0);
	m_vertices[vert].color = colour6;
	m_vertices[vert++].position = Vector3(0, 0, 0);

	m_vertices[vert].color = colour6;
	m_vertices[vert++].position = Vector3(0, 0, 0);
	m_vertices[vert].color = colour6;
	m_vertices[vert++].position = Vector3(0, 0, 0 + m_floatSize);
	m_vertices[vert].color = colour6;
	m_vertices[vert++].position = Vector3(0, 0 + m_floatSize, 0 + m_floatSize);

	BuildImmuatbleVB(device, m_VertexCount, m_vertices);
	BuildImmuatbleIB(device, indices);

	delete[] m_vertices;
	m_vertices = 0;

	delete[] indices;
	indices = 0;

}
void RayCaster::BuildImmuatbleVB(ID3D11Device* device, int _numVerts, void* _vertices)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result = S_OK;
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(VertexLayout) * _numVerts;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = _vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
}
void RayCaster::BuildImmuatbleIB(ID3D11Device* device, void* _indices)
{
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result = S_OK;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = _indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
}

void RayCaster::Shutdown()
{

}
bool RayCaster::Render(ID3D11DeviceContext* deviceContext, Matrix worldMatrix, Matrix projectionMatrix, Matrix viewMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	float clearColor[4] = { 0.f, 0.f, 0.f, 1.f };

#pragma region ShaderParams
	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->proj = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
#pragma endregion
	
	deviceContext->PSSetShaderResources(0, 1, &m_ShaderResource[0]);

#pragma region Render
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_StandardLayout);
	//deviceContext->OMSetRenderTargets(0, &m_RenderTargetView[0], NULL);
	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_StandardVS, NULL, 0);
	deviceContext->PSSetShader(m_StandardPS, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
#pragma endregion

	


	return true;
}

