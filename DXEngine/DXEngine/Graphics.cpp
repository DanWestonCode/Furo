#include "Graphics.h"

const UINT							g_iVolumeSize = 256;	// voxel volume width, height and depth
const UINT							g_vol = 178;

Graphics::Graphics()
{
	m_D3D = nullptr;
	m_ModelShader = nullptr;
	m_VolumeRaycastShader = nullptr;
	m_ModelFront = nullptr;
	m_ModelBack = nullptr;
	m_VolumeTexture = nullptr;

	//sampler
	g_pSamplerLinear = nullptr;
	//vertex and index
	CubeIB = nullptr;
	CubeVB = nullptr;

}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

//Fix for 'warning C4316: object allocated on the heap may not be aligned 16'
//This kept giving me access violation errors using XMMatrix calculations
void* Graphics::operator new(size_t memorySize)
{
	return _aligned_malloc(memorySize, 16);

}

void Graphics::operator delete(void* memoryBlockPtr)
{
	_aligned_free(memoryBlockPtr);
	return;
}


HRESULT Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	HRESULT result = S_OK;

	// Create the Direct3D object.
	m_D3D = new D3D;
	if (!m_D3D)
	{
		return S_FALSE;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return S_FALSE;
	}

	m_ModelShader = new ModelShader;
	m_ModelShader->Initialize(m_D3D->GetDevice(), hwnd);

	m_VolumeRaycastShader = new VolumeRaycastShader;
	m_VolumeRaycastShader->Initialize(m_D3D->GetDevice(), hwnd, screenWidth, screenHeight);

	m_ModelFront = new RenderTexture;
	m_ModelFront->Initialize(m_D3D->GetDevice(), m_D3D->m_ScreenWidth, m_D3D->m_ScreenHeight);

	m_ModelBack = new RenderTexture;
	m_ModelBack->Initialize(m_D3D->GetDevice(), m_D3D->m_ScreenWidth, m_D3D->m_ScreenHeight);

	m_VolumeTexture = new VolumeTexture;
	m_VolumeTexture->Initialize(m_D3D->GetDevice(), 256);

	//Compile Shaders
	CreateSampler(m_D3D->GetDevice());
	CreateCube(m_D3D->GetDevice());

	// Initialize the view matrix
	XMVECTOR eye = XMVectorSet(0.f, 1.5f, -5.0f, 0.f);
	XMVECTOR at = XMVectorSet(0.f, 0.0f, 0.f, 0.f);
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMMATRIX mView = XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up));

	// Initialize the projection matrix
	XMMATRIX mProjection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.f, 0.1f, 10.f));

	// View-projection matrix
	viewProj = XMMatrixMultiply(mProjection, mView);

	return true;

}

void Graphics::Shutdown()
{
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	
	//sampler
	g_pSamplerLinear->Release();

	CubeVB->Release();
	CubeIB->Release();

	m_VolumeRaycastShader->Shutdown();
	m_VolumeRaycastShader = nullptr;
	m_ModelShader->Shutdown();
	m_ModelShader = nullptr;	
	m_ModelFront->Shutdown();
	m_ModelFront = nullptr;
	m_ModelBack->Shutdown();
	m_ModelBack = nullptr;
	m_VolumeTexture->Shutdown();
	m_VolumeTexture = nullptr;

	return;
}

bool Graphics::Frame(float dt)
{
	bool result;
	//update any objects
	//Update(dt);
	// Render the graphics scene.
	result = Render(dt);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render(float dt)
{
	// Clear the back buffer
	//m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &TheBackBufferRenderTargetView, NULL);
	float clearColor[4] = { 0.f, 0.f, 0.f, 1.f };	// red, green, blue, alpha
	m_D3D->GetDeviceContext()->ClearRenderTargetView(m_D3D->m_renderTargetView, clearColor);

	// Set vertex buffer
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	m_D3D->GetDeviceContext()->IASetVertexBuffers(0, 1, &CubeVB, &stride, &offset);
	// Set index buffer
	m_D3D->GetDeviceContext()->IASetIndexBuffer(CubeIB, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	m_D3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the input layout
	m_D3D->GetDeviceContext()->IASetInputLayout(m_ModelShader->GetInputLayout());

	// Rotate the cube around the origin
	XMMATRIX mWorld = XMMatrixIdentity();//XMMatrixRotationY(XM_PIDIV4*dt);
	MatrixBuffer cb;
	cb.mWVP = XMMatrixMultiply(viewProj, mWorld);
	m_D3D->GetDeviceContext()->UpdateSubresource(m_ModelShader->m_MatrixBuffer, 0, NULL, &cb, 0, 0);

	// Render to position textures

	// Set the vertex shader
	m_D3D->GetDeviceContext()->VSSetShader(m_ModelShader->GetVertexShader(), NULL, 0);
	m_D3D->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ModelShader->m_MatrixBuffer);

	// Set the pixel shader
	m_D3D->GetDeviceContext()->PSSetShader(m_ModelShader->GetPixelShader(), NULL, 0);

	// Front-face culling
	m_D3D->GetDeviceContext()->RSSetState(m_D3D->m_backFaceCull);
	m_D3D->GetDeviceContext()->ClearRenderTargetView(m_ModelBack->m_RenderTargetView, clearColor);
	m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_ModelBack->m_RenderTargetView, NULL);
	m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);		// Draw back faces

	// Back-face culling
	m_D3D->GetDeviceContext()->RSSetState(m_D3D->m_FrontFaceCull);
	m_D3D->GetDeviceContext()->ClearRenderTargetView(m_ModelFront->m_RenderTargetView, clearColor);
	m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_ModelFront->m_RenderTargetView, NULL);
	m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);		// Draw front faces

	//// Ray-casting

	// Set the input layout
	m_D3D->GetDeviceContext()->IASetInputLayout(m_ModelShader->GetInputLayout());

	// Render to standard render target
	m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_D3D->m_renderTargetView, NULL);

	// Set the vertex shader
	m_D3D->GetDeviceContext()->VSSetShader(m_VolumeRaycastShader->GetVertexShader(), NULL, 0);
	m_D3D->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ModelShader->m_MatrixBuffer);

	// Set the pixel shader
	m_D3D->GetDeviceContext()->PSSetShader(m_VolumeRaycastShader->GetPixelShader(), NULL, 0);
	m_D3D->GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_VolumeRaycastShader->m_WindowSizeCB);
	//// Set texture sampler
	m_D3D->GetDeviceContext()->PSSetSamplers(0, 1, &g_pSamplerLinear);

	//// Set textures
	m_D3D->GetDeviceContext()->PSSetShaderResources(0, 1, &m_VolumeTexture->m_ShaderResourceView);
	m_D3D->GetDeviceContext()->PSSetShaderResources(1, 1, &m_ModelFront->m_ShaderResourceView);
	m_D3D->GetDeviceContext()->PSSetShaderResources(2, 1, &m_ModelBack->m_ShaderResourceView);

	// Draw the cube
	m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);

	// Un-bind textures
	ID3D11ShaderResourceView *nullRV[3] = { NULL, NULL, NULL };
	m_D3D->GetDeviceContext()->PSSetShaderResources(0, 3, nullRV);

	m_D3D->EndScene();

	return true;
}

void Graphics::CreateSampler(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
}

void Graphics::CreateCube(ID3D11Device* device)
{
	// Create ray-cast vertex and index buffers
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
	hr = (device->CreateBuffer(&bd, &initData, &CubeVB));

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
	hr = (device->CreateBuffer(&bd, &initData, &CubeIB));	
}
