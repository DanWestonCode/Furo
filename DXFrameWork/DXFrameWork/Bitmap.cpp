#include "Bitmap.h"

Bitmap::Bitmap()
{
	m_VertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_Texture = nullptr;
}

Bitmap::Bitmap(const Bitmap& other){}

Bitmap::~Bitmap(){}

HRESULT Bitmap::Initialize(ID3D11Device* _device, int _screenWidth, int _screenHeight, WCHAR* _texture, int _bitmapWidth, int _bitmapHeight, HWND hwnd)
{
	HRESULT result;

	//Set up shader
	// Create the texture shader object.
	//m_TextureShader = new TextureShader;
	// Initialize the texture shader object.
	//result = m_TextureShader->Initialize(_device, hwnd);
	//if (FAILED(result))
	//{
	//	MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
	//	return result;
	//}

	// Store the screen size.
	m_screenWidth = _screenWidth;
	m_screenHeight = _screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = _bitmapWidth;
	m_bitmapHeight = _bitmapHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	InititalizeBuffers(_device);

	result = VertexModel::LoadTexture(_device, _texture);
	/*if (FAILED(result))
	{
	return result;
	}*/

	return S_OK;
}

void Bitmap::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	// Shutdown the vertex and index buffers.
	VertexModel::Shutdown();
	return;
}

void Bitmap::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMMATRIX* worldMatrix, XMMATRIX* viewMatrix, XMMATRIX* orthoMatrix)
{
	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	UpdateBuffers(deviceContext, positionX, positionY);
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	VertexModel::Render(deviceContext);

	//m_TextureShader->Render(deviceContext, GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, GetTexture(), 1.0f);
}

void Bitmap::InititalizeBuffers(ID3D11Device* _device)
{
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	int numTris = 2;
	// Set the number of vertices in the vertex array.
	m_VertexCount = 6 * (numTris - 1) * (numTris - 1);

	// Set the number of indices in the index array.
	m_IndexCount = m_VertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_VertexCount];

	// Create the index array.
	indices = new unsigned long[m_IndexCount];


	

	// Load the index array with data.
	for (i = 0; i < m_IndexCount; i++)
	{
		indices[i] = i;
	}
	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_VertexCount));

	BuildDynamicVB(_device, m_VertexCount, vertices);
	BuildIndexBuffer(_device, indices);

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

bool Bitmap::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// Create the vertex array.
	vertices = new VertexType[m_VertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = Vector3(left, top, 0.0f);  // Top left.
	//vertices[0].texture = Vector2(0.0f, 0.0f);
	vertices[0].color = Color(1.0f, 0.0f,0.0f,1.0f);

	vertices[1].position = Vector3(right, bottom, 0.0f);  // Bottom right.
	//vertices[1].texture = Vector2(1.0f, 1.0f);
	vertices[1].color = Color(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[2].position = Vector3(left, bottom, 0.0f);  // Bottom left.
	//vertices[2].texture = Vector2(0.0f, 1.0f);
	vertices[2].color = Color(1.0f, 0.0f, 0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = Vector3(left, top, 0.0f);  // Top left.
	//vertices[3].texture = Vector2(0.0f, 0.0f);
	vertices[3].color = Color(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[4].position = Vector3(right, top, 0.0f);  // Top right.
	//vertices[4].texture = Vector2(1.0f, 0.0f);
	vertices[4].color = Color(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[5].position = Vector3(right, bottom, 0.0f);  // Bottom right.
	//vertices[5].texture = Vector2(1.0f, 1.0f);
	vertices[5].color = Color(1.0f, 0.0f, 0.0f, 1.0f);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_VertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_VertexBuffer, 0);

	//// Release the vertex array as it is no longer needed.
	//delete[] vertices;
	//vertices = 0;

	return true;
}