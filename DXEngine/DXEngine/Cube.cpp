#include "Cube.h"

Cube::Cube(){}
Cube::~Cube(){}

HRESULT Cube::Initialise(ID3D11Device* _device)
{
	HRESULT hr = S_OK;
	XMFLOAT3 vertices[] =
	{
		XMFLOAT3(-1.f, -1.f, -1.f),
		XMFLOAT3(-1.f, -1.f, 1.f),
		XMFLOAT3(-1.f, 1.f, -1.f),
		XMFLOAT3(-1.f, 1.f, 1.f),
		XMFLOAT3(1.f, -1.f, -1.f),
		XMFLOAT3(1.f, -1.f, 1.f),
		XMFLOAT3(1.f, 1.f, -1.f),
		XMFLOAT3(1.f, 1.f, 1.f),
	};
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(XMFLOAT3) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	hr = (_device->CreateBuffer(&bd, &initData, &m_VertexBuffer));

	// Create index buffer
	WORD indices[] =
	{
		0, 1, 2,
		2, 1, 3,

		0, 4, 1,
		1, 4, 5,

		0, 2, 4,
		4, 2, 6,

		1, 5, 3,
		3, 5, 7,

		2, 3, 6,
		6, 3, 7,

		5, 4, 7,
		7, 4, 6,
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = indices;
	hr = (_device->CreateBuffer(&bd, &initData, &m_IndexBuffer));

	m_scale = XMFLOAT3(4, 8, 4);

	return hr;
}

void Cube::Render(ID3D11DeviceContext* _device)
{
	// Set vertex buffer
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	_device->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	// Set index buffer
	_device->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// Set primitive topology
	_device->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Cube::Shutdown()
{
	VertexObject::Shutdown();
}

void Cube::Update(float dt)
{
	if (InputManager::Instance()->IsKeyDown(DIK_Z))
		m_rot.x += vel*dt;
	if (InputManager::Instance()->IsKeyDown(DIK_X))
		m_rot.y += vel*dt;
	if (InputManager::Instance()->IsKeyDown(DIK_C))
		m_rot.z += vel*dt;

	VertexObject::Update(dt);
}
