#include "Quad.h"

Quad::Quad()
{
	m_ColorShader = nullptr;
	m_ColorVertLayout = nullptr;
	m_Furo = nullptr;
	m_FluidShader = nullptr;
	m_TextureB = nullptr;
	m_TextureA = nullptr;
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
	m_FluidShader = new FluidShader;
	m_FluidShader->Initialize(device, hwnd);

	numTris = 50;
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


	m_TextureA = new RenderTexture;
	m_TextureB = new RenderTexture;
	m_TextureA->Initialize(device, 800, 600);
	m_TextureB->Initialize(device, 800, 600);


	m_TextureA->tag = "TextureA";
	m_TextureB->tag = "TextureB";

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

void Quad::Render(ID3D11DeviceContext* deviceContext, D3D* _d3d)
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
	//m_ColorShader->Render(deviceContext, &m_worldMatrix, m_IndexCount);
	m_FluidShader->Render(deviceContext, &m_worldMatrix, m_IndexCount, m_TextureA, m_TextureB, _d3d->m_renderTargetView);

	
	deviceContext->OMSetRenderTargets(1, &_d3d->m_renderTargetView, NULL);

	m_ColorShader->Render(deviceContext, &m_worldMatrix, m_IndexCount, m_TextureB);
	
 	RenderTexture* temp = new RenderTexture;
	//temp = m_TextureA;
	//m_TextureA = m_TextureB;
	//m_TextureB = temp;

}

void Quad::Shutdown()
{
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
	}
	if (m_ColorVertLayout)
	{
		m_ColorVertLayout = nullptr;
		delete m_ColorVertLayout;
	}
	if (m_Furo)
	{
		m_Furo->Shutdown();
		delete m_Furo;
	}
	if (m_FluidShader)
	{
		m_FluidShader->Shutdown();
		delete m_FluidShader;
	}
	if (m_TextureB)
	{
		m_TextureB->Shutdown();
		delete m_TextureB;
	}
	if (m_TextureA)
	{
		m_TextureA->Shutdown();
		delete m_TextureA;
	}

	VertexObject::Shutdown();
}

void Quad::Update(float dt)
{
	//m_Furo->Run(dt);
	//if (InputManager::Instance()->IsKeyDown(DIK_A))
	//	m_rot.x += vel*dt;

	//m_Furo->m_textureFluid->Clear();
	//if (InputManager::Instance()->IsKeyDown(DIK_Q))
	//{
	//	for (int i = 35; i < 40; i++)
	//	{
	//		m_Furo->m_textureFluid->SetDensity(i, 1, 1.0f);

	//	}
	//}
	//if (InputManager::Instance()->IsKeyDown(DIK_E))
	//{
	//	for (int i = 35; i < 40; i++)
	//	{

	//		m_Furo->m_textureFluid->SetVelX(i, 1, 100.0f);
	//		m_Furo->m_textureFluid->SetVelY(i, 1, 100.0f);
	//		m_Furo->m_textureFluid->SetVelX(i, 1, 100.0f);
	//		m_Furo->m_textureFluid->SetVelY(i, 1, 100.0f);
	//		m_Furo->m_textureFluid->SetVelX(i, 2, 100.0f);
	//		m_Furo->m_textureFluid->SetVelY(i, 2, 100.0f);
	//		m_Furo->m_textureFluid->SetVelX(i, 2, 100.0f);
	//		m_Furo->m_textureFluid->SetVelY(i, 2, 100.0f);

	//		m_Furo->m_textureFluid->SetVelX(i, 3, 100.0f);
	//		m_Furo->m_textureFluid->SetVelY(i, 3, 100.0f);
	//		m_Furo->m_textureFluid->SetVelX(i, 3, 100.0f);
	//		m_Furo->m_textureFluid->SetVelY(i, 3, 100.0f);
	//	}

	//}

	//UpdateFluid(m_Furo->m_textureFluid->GetDensity());

	VertexObject::Update(dt);
}

void Quad::UpdateFluid(float* dens)
{
	int vert = 0;
	for (int i = 0; i < (numTris - 1); i++)
	{
		for (int j = 0; j < (numTris - 1); j++)
		{
			float x = dens[i * (numTris + 2) + j];
			x *= 255;

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