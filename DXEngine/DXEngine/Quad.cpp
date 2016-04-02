#include "Quad.h"
#include "Camera.h"
Quad::Quad()
{
	m_ColorShader = nullptr;
	m_ColorVertLayout = nullptr;
	m_fluid = nullptr;

	veloMulti = 100;
	densityMulti = 10.0f;
}

Quad::Quad(const Quad& other)
{
}

Quad::~Quad()
{

}

HRESULT Quad::Initialise(D3D* _d3d, HWND hwnd)
{
	HRESULT result;

	//Set up shader
	//Create the texture shader object.
	m_ColorShader = new ColourShader;
	// Initialize the texture shader object.
	result = m_ColorShader->Initialize(_d3d->GetDevice(), hwnd);

	numTris = 120;
	m_VertexCount = 6 * (numTris - 1) * (numTris - 1);
	// Set the number of indices in the index array.
	m_IndexCount = m_VertexCount;

	// Create the vertex array.
	m_ColorVertLayout = new ColorVL[m_VertexCount];

	indices = new unsigned long[m_IndexCount];
	//set up indices
	for (int i = 0; i < m_VertexCount; i++)
	{
		indices[i] = i;
	}

	int vert = 0;
	XMFLOAT4 colour = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
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
	BuildDynamicVB(_d3d->GetDevice(), m_VertexCount, m_ColorVertLayout, sizeof(ColorVL) * m_VertexCount);
	BuildIndexBuffer(_d3d->GetDevice(), indices);

	delete[] indices;
	indices = 0;

	m_fluid = new StamSolver2D;
	if (!m_fluid)
	{
		return S_FALSE;
	}

	m_fluid->Initialize(numTris);

	TwAddSeparator(_d3d->m_TwBar, "Simulation Variables", "");

	TwAddVarRW(_d3d->m_TwBar, "Density", TW_TYPE_FLOAT, &densityMulti, "");
	TwAddVarRW(_d3d->m_TwBar, "Velocity", TW_TYPE_FLOAT, &veloMulti, "");

	return S_OK;
}

void Quad::Render(D3D* _d3d)
{
	HRESULT hr;
	ColorVL* colourPtr;
	D3D11_MAPPED_SUBRESOURCE resource;

	hr = _d3d->GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	colourPtr = (ColorVL*)resource.pData;
	memcpy(colourPtr, (void*)m_ColorVertLayout, (sizeof(ColorVL) * m_VertexCount));
	_d3d->GetDeviceContext()->Unmap(m_VertexBuffer, 0);

	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(ColorVL);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_d3d->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_d3d->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_d3d->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//render shader
	m_ColorShader->Render(_d3d->GetDeviceContext(), &m_worldMatrix, m_IndexCount);

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
	if (m_fluid)
	{
		m_fluid->Shutdown();
		delete m_fluid;
	}

	VertexObject::Shutdown();
}

void Quad::Update(float dt, HWND hwnd)
{
	m_fluid->Run(dt);
	m_fluid->Clear();

	m_fluid->SetDensity((int)numTris / 2.0f, 2, densityMulti);
	m_fluid->SetVelX((int)numTris / 2.0f, 2, veloMulti);
	m_fluid->SetVelY((int)numTris / 2.0f, 2, veloMulti);

	//Copy the fluid into the struct ready for rendering
	UpdateFluid(m_fluid->m_density);

	//update base class
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