#include "Quad.h"
#include "FluidHelper.h"

Quad::Quad()
{
	m_VertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_TextureShader = nullptr;

	
}

Quad::Quad(const Quad& other)
{
}

Quad::~Quad()
{

}

HRESULT Quad::Initialize(ID3D11Device* device, WCHAR* texture, HWND hwnd)
{
	HRESULT result;
	unsigned long* indices;

	//Set up shader
	//Create the texture shader object.
	m_TextureShader = new TextureShader;
	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(device, hwnd);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return result;
	}

	numTris = 100;
	m_VertexCount = 6 * (numTris - 1) * (numTris - 1);
	// Set the number of indices in the index array.
	m_IndexCount = m_VertexCount;

	// Create the vertex array.
	m_VerticesTextureVL = new TextureVL[m_VertexCount];

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
	//create vertices
	for (int i = 0; i < (numTris - 1); i++)
	{
		for (int j = 0; j < (numTris - 1); j++)
		{	
			m_VerticesTextureVL[vert].color = colour;
			m_VerticesTextureVL[vert].texture = Vector2((float)i / (float)numTris, (float)j / (float)numTris);
			m_VerticesTextureVL[vert++].position = Vector3((float)i, (float)j, 0.0f);

			m_VerticesTextureVL[vert].color = colour;
			m_VerticesTextureVL[vert].texture = Vector2((float)i / (float)numTris, (float)(j + 1) / (float)numTris);
			m_VerticesTextureVL[vert++].position = Vector3((float)i, (float)(j + 1), 0.0f);

			m_VerticesTextureVL[vert].color = colour;
			m_VerticesTextureVL[vert].texture = Vector2((float)(i + 1) / (float)numTris, (float)j / (float)numTris);
			m_VerticesTextureVL[vert++].position = Vector3((float)(i + 1), (float)j, 0.0f);

			m_VerticesTextureVL[vert].color = colour;
			m_VerticesTextureVL[vert].texture = Vector2((float)(i + 1) / (float)numTris, (float)j / (float)numTris);
			m_VerticesTextureVL[vert++].position = Vector3((float)(i + 1), (float)j, 0.0f);

			m_VerticesTextureVL[vert].color = colour;
			m_VerticesTextureVL[vert].texture = Vector2((float)i / (float)numTris, (float)(j + 1) / (float)numTris);
			m_VerticesTextureVL[vert++].position = Vector3((float)i, (float)(j + 1), 0.0f);

			m_VerticesTextureVL[vert].color = colour;
			m_VerticesTextureVL[vert].texture = Vector2((float)(i + 1) / (float)numTris, (float)(j + 1) / (float)numTris);
			m_VerticesTextureVL[vert++].position = Vector3((float)(i + 1), (float)(j + 1), 0.0f);
		}
	}

	BuildDynamicVB(device, m_VertexCount, m_VerticesTextureVL);
	BuildIndexBuffer(device, indices);	

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	/*delete[] m_VerticesTextureVL;
	m_VerticesTextureVL = 0;*/

	delete[] indices;
	indices = 0;
	
	// Load the texture for this model.
	result = VertexModel::LoadTexture(device, texture);
	if (FAILED(result))
	{
		return result;
	}

	VertexModel::Initialize(device, texture);

	return S_OK;
}

void Quad::Render(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix)
{
	HRESULT hr;
	TextureVL* verticesPtr;
	D3D11_MAPPED_SUBRESOURCE resource;

	hr = deviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	verticesPtr = (TextureVL*)resource.pData;
	memcpy(verticesPtr, (void*)m_VerticesTextureVL, (sizeof(TextureVL) * m_VertexCount));
	deviceContext->Unmap(m_VertexBuffer, 0);

	VertexModel::Render(deviceContext);

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
	m_TextureShader->Render(deviceContext, GetIndexCount(), &m_worldMatrix, viewMatrix, projectionMatrix, GetTexture(), 1.0f);
}

void Quad::Shutdown()
{
	ReleaseTexture();
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	VertexModel::Shutdown();
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
			//x *= 255;

			Color colour = Color(x, x, x, x);

			m_VerticesTextureVL[vert].color = colour;
			vert++;
			m_VerticesTextureVL[vert].color = colour;
			vert++;
			m_VerticesTextureVL[vert].color = colour;
			vert++;
			m_VerticesTextureVL[vert].color = colour;
			vert++;
			m_VerticesTextureVL[vert].color = colour;
			vert++;
			m_VerticesTextureVL[vert].color = colour;
			vert++;
		}
	}
}

void Quad::Update(float dt)
{
	if (InputManager::Instance()->IsKeyDown(DIK_A))
		m_rot.x += vel*dt;

	VertexModel::Update(dt);
}