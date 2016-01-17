#include "Quad.h"

Quad::Quad()
{
	m_ColorShader = nullptr;
	m_ColorVertLayout = nullptr;
	m_Furo = nullptr;
}

Quad::Quad(const Quad& other)
{
}

Quad::~Quad()
{

}

HRESULT Quad::Initialise(ID3D11Device* device, HWND hwnd)
{
	HRESULT result;
	unsigned long* indices;

	//Set up shader
	//Create the texture shader object.
	m_ColorShader = new ColourShader;
	// Initialize the texture shader object.
	result = m_ColorShader->Initialize(device, hwnd);

	numTris = 100;
	m_VertexCount = 6 * (numTris - 1) * (numTris - 1);
	// Set the number of indices in the index array.
	m_IndexCount = m_VertexCount;

	// Create the vertex array.
	m_ColorVertLayout = new ColorVL[m_VertexCount];

	// Create the index array.
	indices = new unsigned long[m_IndexCount];

	//set up indices
	for (int i = 0; i < m_VertexCount; i++)
	{
		indices[i] = i;
	}

	int vert = 0;
	XMFLOAT4 colour = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	//create vertices
	for (int i = 0; i < (numTris - 1); i++)
	{
		for (int j = 0; j < (numTris - 1); j++)
		{
			m_ColorVertLayout[vert].color = colour;
			m_ColorVertLayout[vert++].position = XMFLOAT3((float)i, (float)j, 0.0f);

			m_ColorVertLayout[vert].color = colour;
			m_ColorVertLayout[vert++].position = XMFLOAT3((float)i, (float)(j + 1), 0.0f);

			m_ColorVertLayout[vert].color = colour;
			m_ColorVertLayout[vert++].position = XMFLOAT3((float)(i + 1), (float)j, 0.0f);

			m_ColorVertLayout[vert].color = colour;
			m_ColorVertLayout[vert++].position = XMFLOAT3((float)(i + 1), (float)j, 0.0f);

			m_ColorVertLayout[vert].color = colour;
			m_ColorVertLayout[vert++].position = XMFLOAT3((float)i, (float)(j + 1), 0.0f);

			m_ColorVertLayout[vert].color = colour;
			m_ColorVertLayout[vert++].position = XMFLOAT3((float)(i + 1), (float)(j + 1), 0.0f);
		}
	}

	BuildDynamicVB(device, m_VertexCount, m_ColorVertLayout, sizeof(ColorVL) * m_VertexCount);
	BuildIndexBuffer(device, indices);

	//sizeof(_layout) * _numVerts

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	/*delete[] m_VerticesTextureVL;
	m_VerticesTextureVL = 0;*/

	delete[] indices;
	indices = 0;

	m_Furo = new Furo;
	if (!m_Furo)
	{
		return S_FALSE;
	}
	m_Furo->Initialize(Furo::FluidField::TwoDimensional, 100, 0.1f);

	return S_OK;
}

void Quad::Render(ID3D11DeviceContext* deviceContext)
{
	HRESULT hr;
	ColorVL* colourPtr;
	D3D11_MAPPED_SUBRESOURCE resource;

	hr = deviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	colourPtr = (ColorVL*)resource.pData;
	memcpy(colourPtr, (void*)m_ColorVertLayout, (sizeof(ColorVL) * m_VertexCount));
	deviceContext->Unmap(m_VertexBuffer, 0);

	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(ColorVL);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render the model using the texture shader.
	m_ColorShader->Render(deviceContext, &m_worldMatrix, m_IndexCount);
}

void Quad::Shutdown()
{
	VertexObject::Shutdown();
}

//void Quad::UpdateTexture(float* dens)
//{
//	//float d = dens[FluidHelper::GetIndex(numTris, 1, 1)];
//	int vert = 0;
//	for (int i = 0; i < (numTris - 1); i++)
//	{
//		for (int j = 0; j < (numTris - 1); j++)
//		{
//			float x = dens[FluidHelper::GetIndex(numTris, i, j)];
//			//x *= 255;
//
//			Color colour = Color(x, x, x, x);
//
//			m_VerticesTextureVL[vert].color = colour;
//			vert++;
//			m_VerticesTextureVL[vert].color = colour;
//			vert++;
//			m_VerticesTextureVL[vert].color = colour;
//			vert++;
//			m_VerticesTextureVL[vert].color = colour;
//			vert++;
//			m_VerticesTextureVL[vert].color = colour;
//			vert++;
//			m_VerticesTextureVL[vert].color = colour;
//			vert++;
//		}
//	}
//}

void Quad::Update(float dt)
{
	m_Furo->Run(dt);
	if (InputManager::Instance()->IsKeyDown(DIK_A))
		m_rot.x += vel*dt;

	m_Furo->GetFluid()->Clear();
	if (InputManager::Instance()->IsKeyDown(DIK_Q))
	{
		for (int i = 0; i < 100; i++)
		{
			m_Furo->GetFluid()->SetDensity(i, 1, 1.0f);

		}
	}
	if (InputManager::Instance()->IsKeyDown(DIK_E))
	{
		for (int i = 0; i < 100; i++)
		{

			m_Furo->GetFluid()->SetVelX(i, 1, 100.0f);
			m_Furo->GetFluid()->SetVelY(i, 1, 100.0f);
			m_Furo->GetFluid()->SetVelX(i, 1, 100.0f);
			m_Furo->GetFluid()->SetVelY(i, 1, 100.0f);
			m_Furo->GetFluid()->SetVelX(i, 2, 100.0f);
			m_Furo->GetFluid()->SetVelY(i, 2, 100.0f);
			m_Furo->GetFluid()->SetVelX(i, 2, 100.0f);
			m_Furo->GetFluid()->SetVelY(i, 2, 100.0f);

			m_Furo->GetFluid()->SetVelX(i, 3, 100.0f);
			m_Furo->GetFluid()->SetVelY(i, 3, 100.0f);
			m_Furo->GetFluid()->SetVelX(i, 3, 100.0f);
			m_Furo->GetFluid()->SetVelY(i, 3, 100.0f);
		}

	}

	UpdateFluid(m_Furo->GetFluid()->GetDensity());

	VertexObject::Update(dt);
}

void Quad::UpdateFluid(float* dens)
{
	int vert = 0;
	for (int i = 0; i < (numTris - 1); i++)
	{
		for (int j = 0; j < (numTris - 1); j++)
		{
			float x = dens[FluidHelper::GetIndex(numTris, i, j)];
			//x *= 255;

			XMFLOAT4 colour = XMFLOAT4(x, x, x, x);

			m_ColorVertLayout[vert++].color = colour;
			m_ColorVertLayout[vert++].color = colour;
			m_ColorVertLayout[vert++].color = colour;
			m_ColorVertLayout[vert++].color = colour;
			m_ColorVertLayout[vert++].color = colour;
			m_ColorVertLayout[vert++].color = colour;
		}
	}
}