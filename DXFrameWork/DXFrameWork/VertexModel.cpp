#include "VertexModel.h"

VertexModel::VertexModel()
{
	m_VertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_Texture = nullptr;
}

VertexModel::VertexModel(const VertexModel& other){}

VertexModel::~VertexModel(){}

ID3D11ShaderResourceView* VertexModel::GetTexture()
{
	return m_Texture;
}

void VertexModel::Shutdown()
{
	// Release the index buffer.
	if (m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = 0;
	}
	return;
}

void VertexModel::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

//Build a static Vertex buffer for vertices
HRESULT VertexModel::BuildDynamicVB(ID3D11Device* device, int _numVerts, void* _vertices)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result = S_OK;
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(TextureVL) * _numVerts;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = _vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);

	return result;	
}

//Build a dynamic vertex buffer for vertices
HRESULT VertexModel::BuildStaticVB(ID3D11Device* device, int _numVerts, void* _vertices)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result = S_OK;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(TextureVL) * _numVerts;
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

	return result;
}

HRESULT VertexModel::BuildImmuatbleVB(ID3D11Device* device, int _numVerts, void* _vertices)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result = S_OK;
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(ColorVL) * _numVerts;
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

	return result;
}

HRESULT VertexModel::BuildIndexBuffer(ID3D11Device* device, void* _indices)
{
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result = S_OK;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
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

	return result;
}

HRESULT VertexModel::BuildImmuatbleIB(ID3D11Device* device, void* _indices)
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

	return result;
}


void VertexModel::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	//unsigned int stride;
	//unsigned int offset;

	//// Set vertex buffer stride and offset.
	//stride = sizeof(VertexType);
	//offset = 0;

	//// Set the vertex buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	//// Set the index buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

//helper function
int VertexModel::GetIndexCount()
{
	return m_IndexCount;
}

HRESULT VertexModel::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	// Initialize the texture object.
	m_Texture = TextureLoader::Instance()->LoadTexture(device, filename);
	if (!m_Texture)
	{
		return S_FALSE;
	}

	return S_OK;
}

void VertexModel::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
