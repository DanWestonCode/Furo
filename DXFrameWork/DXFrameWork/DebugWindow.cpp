#include "DebugWindow.h"


DebugWindow::DebugWindow()
{
	m_VertexBuffer = 0;
	m_IndexBuffer = 0;
}


DebugWindow::DebugWindow(const DebugWindow& other)
{
}

DebugWindow::~DebugWindow()
{
}


HRESULT DebugWindow::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight, HWND hwnd)
{	
	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Initialize the vertex and index buffers.
	InitializeBuffers(device, hwnd);

	return true;
}


void DebugWindow::Shutdown()
{
	VertexModel::Shutdown();

	return;
}


void DebugWindow::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMMATRIX* worldMatrix, XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix, ID3D11ShaderResourceView* _RenderTexture)
{
	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	UpdateBuffers(deviceContext, positionX, positionY);

	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(TextureVL);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render the model using the texture shader.
	m_TextureShader->Render(deviceContext, GetIndexCount(), &m_worldMatrix, viewMatrix, projectionMatrix, _RenderTexture, 1.0f);
}


void DebugWindow::InitializeBuffers(ID3D11Device* device, HWND hwnd)
{
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//Set up shader
	//Create the texture shader object.
	m_TextureShader = new TextureShader;
	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(device, hwnd);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
	}


	// Set the number of vertices in the vertex array.
	m_VertexCount = 6;

	// Set the number of indices in the index array.
	m_IndexCount = m_VertexCount;

	// Create the vertex array.
	m_VerticesTextureVL = new TextureVL[m_VertexCount];

	// Create the index array.
	indices = new unsigned long[m_IndexCount];
	
	// Initialize vertex array to zeros at first.
	memset(m_VerticesTextureVL, 0, (sizeof(TextureVL) * m_VertexCount));

	// Load the index array with data.
	for (i = 0; i < m_IndexCount; i++)
	{
		indices[i] = i;
	}

	BuildDynamicVB(device, m_VertexCount, m_VerticesTextureVL);
	BuildIndexBuffer(device, indices);

	/*delete[] m_VerticesTextureVL;
	vertices = 0;*/

	delete[] indices;
	indices = 0;
}


bool DebugWindow::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	TextureVL* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	TextureVL* verticesPtr;
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
	vertices = new TextureVL[m_VertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = Vector3(left, top, 0.0f);  // Top left.
	vertices[0].texture = Vector2(0.0f, 0.0f);
	vertices[0].color = Color(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[1].position = Vector3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = Vector2(1.0f, 1.0f);
	vertices[1].color = Color(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[2].position = Vector3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = Vector2(0.0f, 1.0f);
	vertices[2].color = Color(0.0f, 0.0f, 1.0f, 1.0f);

	// Second triangle.
	vertices[3].position = Vector3(left, top, 0.0f);  // Top left.
	vertices[3].texture = Vector2(0.0f, 0.0f);
	vertices[3].color = Color(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[4].position = Vector3(right, top, 0.0f);  // Top right.
	vertices[4].texture = Vector2(1.0f, 0.0f);
	vertices[4].color = Color(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[5].position = Vector3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = Vector2(1.0f, 1.0f);
	vertices[5].color = Color(0.0f, 0.0f, 1.0f, 1.0f);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (TextureVL*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(TextureVL) * m_VertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_VertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}


