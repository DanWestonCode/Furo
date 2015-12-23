#include "Quad.h"
#include "FluidHelper.h"

Quad::Quad()
{
	m_VertexBuffer = 0;
	m_IndexBuffer = 0;
}

Quad::Quad(const Quad& other)
{
}

Quad::~Quad()
{

}

HRESULT Quad::Initialize(ID3D11Device* device, WCHAR* texture)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	unsigned long* indices;
	
	numTris = 100;
	m_VertexCount = 6 * (numTris - 1) * (numTris - 1);
	// Set the number of indices in the index array.
	m_IndexCount = m_VertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_VertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_IndexCount];
	if (!indices)
	{
		return false;
	}

	//set up indices
	for (int i = 0; i < m_VertexCount; i++)
	{
		indices[i] = i;
	}

	int vert = 0;
	Color colour = Color(1.0f, 0.0f, 0.0f, 1.0f);
	Color colour2 = Color(1.0f, 0.0f, 0.0f, 0.5f);
	//create vertices
	for (int i = 0; i < (numTris - 1); i++)
	{
		for (int j = 0; j < (numTris - 1); j++)
		{	
			vertices[vert].color = colour;
			vertices[vert].texture = Vector2((float)i / (float)numTris, (float)j / (float)numTris);
			vertices[vert++].position = Vector3((float)i, (float)j, 0.0f);
			vertices[vert].color = colour;
			vertices[vert].texture = Vector2((float)i / (float)numTris, (float)(j + 1) / (float)numTris);
			vertices[vert++].position = Vector3((float)i, (float)(j + 1), 0.0f);

			vertices[vert].color = colour2;
			vertices[vert].texture = Vector2((float)(i + 1) / (float)numTris, (float)j / (float)numTris);
			vertices[vert++].position = Vector3((float)(i + 1), (float)j, 0.0f);
			vertices[vert].color = colour2;
			vertices[vert].texture = Vector2((float)(i + 1) / (float)numTris, (float)j / (float)numTris);
			vertices[vert++].position = Vector3((float)(i + 1), (float)j, 0.0f);

			vertices[vert].color = colour2;
			vertices[vert].texture = Vector2((float)i / (float)numTris, (float)(j + 1) / (float)numTris);
			vertices[vert++].position = Vector3((float)i, (float)(j + 1), 0.0f);
			vertices[vert].color = colour2;
			vertices[vert].texture = Vector2((float)(i + 1) / (float)numTris, (float)(j + 1) / (float)numTris);
			vertices[vert++].position = Vector3((float)(i + 1), (float)(j + 1), 0.0f);
		}
	}

	BuildDynamicVB(device, m_VertexCount, vertices);
	BuildIndexBuffer(device, indices);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	/*delete[] vertices;
	vertices = 0;*/
	delete[] indices;
	indices = 0;
	
	// Load the texture for this model.
	result = VertexModel::LoadTexture(device, texture);
	if (result != S_OK)
	{
		return S_FALSE;
	}

	VertexModel::Initialize(device, texture);

	return S_OK;
}

void Quad::Render(ID3D11DeviceContext* deviceContext)
{
	HRESULT hr;
	VertexType* verticesPtr;
	D3D11_MAPPED_SUBRESOURCE resource;
	hr = deviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	verticesPtr = (VertexType*)resource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_VertexCount));
	deviceContext->Unmap(m_VertexBuffer, 0);

	VertexModel::Render(deviceContext);
}

void Quad::Shutdown()
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

void Quad::UpdateTexture(float* dens)
{
	//float d = dens[FluidHelper::GetIndex(numTris, 1, 1)];
	int vert = 0;
	for (int i = 0; i < (numTris - 1); i++)
	{
		for (int j = 0; j < (numTris - 1); j++)
		{
			float x = dens[FluidHelper::GetIndex(numTris, i, j)];
			x *= 255;

			Color colour = Color(1.0f, x, x, 1.0f);

			vertices[vert].color = colour;
			vert++;
			vertices[vert].color = colour;
			vert++;
			vertices[vert].color = colour;
			vert++;
			vertices[vert].color = colour;
			vert++;
			vertices[vert].color = colour;
			vert++;
			vertices[vert].color = colour;
			vert++;
		}
	}
}