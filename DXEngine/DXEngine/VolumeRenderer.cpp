#include "VolumeRenderer.h"
const UINT							g_iVolumeSize = 64;
VolumeRenderer::VolumeRenderer()
{
	m_ModelShader = nullptr;
	m_VolumeRaycastShader = nullptr;
	m_ModelFront = nullptr;
	m_ModelBack = nullptr;
	m_VolumeTexture = nullptr;
	m_cube = nullptr;
	g_pSamplerLinear = nullptr;
}
void* VolumeRenderer::operator new(size_t memorySize)
{
	return _aligned_malloc(memorySize, 16);

}
void VolumeRenderer::operator delete(void* memoryBlockPtr)
{
	_aligned_free(memoryBlockPtr);
	return;
}
VolumeRenderer::VolumeRenderer(const VolumeRenderer& other){}
VolumeRenderer::~VolumeRenderer(){}

HRESULT VolumeRenderer::Initialize(D3D* _d3d, HWND _hWnd, int _width, int _height)
{
	HRESULT result = S_OK;

	m_ModelShader = new ModelShader;
	m_ModelShader->Initialize(_d3d->GetDevice(), _hWnd);

	m_VolumeRaycastShader = new VolumeRaycastShader;
	m_VolumeRaycastShader->Initialize(_d3d->GetDevice(), _hWnd, _width, _height);

	m_ModelFront = new RenderTexture;
	m_ModelFront->Initialize(_d3d->GetDevice(), _width, _height);

	m_ModelBack = new RenderTexture;
	m_ModelBack->Initialize(_d3d->GetDevice(), _width, _height);

	m_VolumeTexture = new VolumeTexture;
	m_VolumeTexture->Initialize(_d3d, g_iVolumeSize);

	m_cube = new Cube;
	m_cube->Initialise(_d3d->GetDevice());

	CreateSampler(_d3d->GetDevice());

	m_props.g_iMaxIterations = 128;
	m_props.VolumeColor = XMFLOAT4(1,0,1,1);

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.ByteWidth = sizeof(VolumeRendererProps);
	_d3d->GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_VolumeRendererPropsBuffer);

	/*TwStructMember _VolumeRendererProps[] = {
		{ "Volume Render Iterations", TW_TYPE_FLOAT, offsetof(VolumeRendererProps, g_iMaxIterations), "min=1 max=1000 step=1" },
		{ "Volume Colour", TW_TYPE_COLOR4F, offsetof(VolumeRendererProps, VolumeColor), "min=0.1 max=1 step=0.1" }
		};

		TwAddVarRW(_d3d->m_TwBar, "Volume Renderer Properties", TwDefineStruct("Simulation", _VolumeRendererProps, 2, sizeof(VolumeRendererProps), nullptr, nullptr), &m_props, NULL);
		*/
	return result;
}

void VolumeRenderer::CreateSampler(ID3D11Device* device)
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
	sampDesc.MaxAnisotropy = 16;
	hr = device->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
}

void VolumeRenderer::Shutdown()
{
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
	m_cube->Shutdown();
	m_cube = nullptr;
	g_pSamplerLinear->Release();
	g_pSamplerLinear = nullptr;
}

void VolumeRenderer::Update(float dt, D3D* _d3d)
{
	m_cube->Update(dt);
	m_VolumeTexture->Update(_d3d->GetDevice(), _d3d->GetDeviceContext(), g_iVolumeSize, dt);
}

void VolumeRenderer::Render(D3D* m_D3D)
{
	float ClearBackBuffer[4] = { 0.f, 0.f, 0.f, 1.f };
	float ClearRenderTarget[4] = { 0.f, 0.f, 0.f, 1.f };

	m_VolumeTexture->Render(m_D3D->GetDeviceContext());

	m_cube->Render(m_D3D->GetDeviceContext());

	//// Set the input layout
	m_D3D->GetDeviceContext()->IASetInputLayout(m_ModelShader->GetInputLayout());

	// Rotate the cube around the origin
	XMMATRIX mWorld = m_cube->m_worldMatrix;//XMMatrixIdentity();//XMMatrixRotationY(XM_PIDIV4*dt);

	MatrixBuffer cb;
	cb.mWVP = XMMatrixMultiply(Camera::Instance()->GetViewProj(), mWorld);
	m_D3D->GetDeviceContext()->UpdateSubresource(m_ModelShader->m_MatrixBuffer, 0, NULL, &cb, 0, 0);

	// Render to position textures

	// Set the vertex shader
	m_D3D->GetDeviceContext()->VSSetShader(m_ModelShader->GetVertexShader(), NULL, 0);
	m_D3D->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ModelShader->m_MatrixBuffer);

	// Set the pixel shader
	m_D3D->GetDeviceContext()->PSSetShader(m_ModelShader->GetPixelShader(), NULL, 0);

	// Front-face culling
	m_D3D->GetDeviceContext()->RSSetState(m_D3D->m_backFaceCull);
	m_D3D->GetDeviceContext()->ClearRenderTargetView(m_ModelBack->m_RTV, ClearRenderTarget);
	m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_ModelBack->m_RTV, NULL);
	m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);		// Draw back faces

	// Back-face culling
	m_D3D->GetDeviceContext()->RSSetState(m_D3D->m_FrontFaceCull);
	m_D3D->GetDeviceContext()->ClearRenderTargetView(m_ModelFront->m_RTV, ClearRenderTarget);
	m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_ModelFront->m_RTV, NULL);
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
	m_D3D->GetDeviceContext()->PSSetConstantBuffers(1, 1, &m_VolumeRendererPropsBuffer);

	//// Set texture sampler
	m_D3D->GetDeviceContext()->PSSetSamplers(0, 1, &g_pSamplerLinear);

	//// Set textures
	m_D3D->GetDeviceContext()->PSSetShaderResources(0, 1, &m_VolumeTexture->m_fluidShader->m_DensitySRV[0]);//
	//m_VolumeTexture->m_fluidShader->m_boundarySRV);
	m_D3D->GetDeviceContext()->PSSetShaderResources(1, 1, &m_ModelFront->m_SRV);
	m_D3D->GetDeviceContext()->PSSetShaderResources(2, 1, &m_ModelBack->m_SRV);

	// Draw the cube
	m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);

	 //Un-bind textures
	ID3D11ShaderResourceView *nullRV[3] = { NULL, NULL, NULL };
	m_D3D->GetDeviceContext()->PSSetShaderResources(0, 3, nullRV);
}