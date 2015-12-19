#include "Quad.h"

Quad::Quad()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

Quad::Quad(const Quad& other)
{
}

Quad::~Quad()
{

}

bool Quad::Initialize(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	unsigned long* indices;
	
	int numTris = 2;
	m_vertexCount = 6 * (numTris - 1) * (numTris - 1);
	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//set up incides
	for (int i = 0; i < m_vertexCount; i++)
	{
		indices[i] = i;
	}

	int vert = 0;
	Color colour = Color(0.0f, 0.0f, 1.0f, 1.0f);
	//create vertices
	for (int i = 0; i < (numTris - 1); i++)
	{
		for (int j = 0; j < (numTris - 1); j++)
		{	
			vertices[vert].color = colour;
			vertices[vert++].position = Vector3((float)i, (float)j, 0.0f);
			vertices[vert].color = colour;
			vertices[vert++].position = Vector3((float)i, (float)(j + 1), 0.0f);

			vertices[vert].color = colour;
			vertices[vert++].position = Vector3((float)(i + 1), (float)j, 0.0f);
			vertices[vert].color = colour;
			vertices[vert++].position = Vector3((float)(i + 1), (float)j, 0.0f);

			vertices[vert].color = colour;
			vertices[vert++].position = Vector3((float)i, (float)(j + 1), 0.0f);
			vertices[vert].color = colour;
			vertices[vert++].position = Vector3((float)(i + 1), (float)(j + 1), 0.0f);
		}
	}

	BuildDynamicVB(device, m_vertexCount, vertices);
	BuildIndexBuffer(device, indices);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	/*delete[] vertices;
	vertices = 0;*/
	delete[] indices;
	indices = 0;

	return true;
}

void Quad::Render(ID3D11DeviceContext* deviceContext)
{
	HRESULT hr;
	VertexType* verticesPtr;
	D3D11_MAPPED_SUBRESOURCE resource;
	hr = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	verticesPtr = (VertexType*)resource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	//memcpy(resource.pData, vertices, sizeof(vertices)*m_vertexCount);
	deviceContext->Unmap(m_vertexBuffer, 0);

	VertexModel::Render(deviceContext);
}

void Quad::Shutdown()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}