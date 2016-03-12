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

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	//camera buffer
	bufferDesc.ByteWidth = sizeof(CamBuffer);
	result = _d3d->GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_CamBuffer);

	//object buffer 
	bufferDesc.ByteWidth = sizeof(ObjectBuffer);
	result = _d3d->GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_ObjectBuffer);

	//Fluid buffer 
	bufferDesc.ByteWidth = sizeof(FluidBuffer);
	result = _d3d->GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_FluidBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CamBuffer* camPtr;
	ObjectBuffer* objPtr;
	FluidBuffer* fluidPtr;

	//Cam Buffer
	result = _d3d->GetDeviceContext()->Map(m_CamBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CamBuffer*)mappedResource.pData;
	camPtr->CameraPos = Camera::Instance()->m_pos;
	_d3d->GetDeviceContext()->Unmap(m_CamBuffer, 0);

	//Object Buffer
	result = _d3d->GetDeviceContext()->Map(m_ObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	objPtr = (ObjectBuffer*)mappedResource.pData;
	objPtr->ObjectPos = m_cube->m_pos;
	objPtr->ObjectScale = m_cube->m_scale;
	_d3d->GetDeviceContext()->Unmap(m_ObjectBuffer, 0);

	//fluid buffer
	result = _d3d->GetDeviceContext()->Map(m_FluidBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	fluidPtr = (FluidBuffer*)mappedResource.pData;
	fluidPtr->Absoprtion = 60;
	fluidPtr->Samples = 64;
	_d3d->GetDeviceContext()->Unmap(m_FluidBuffer, 0);

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
	float ClearRenderTarget[4] = { 0.f, 1.f, 0.f, 1.f };

	// Render Cube
	m_cube->Render(m_D3D->GetDeviceContext());

	// Set the input layout
	m_D3D->GetDeviceContext()->IASetInputLayout(m_ModelShader->GetInputLayout());

	XMMATRIX mWorld = m_cube->m_worldMatrix;

	MatrixBuffer cb;
	cb.mWVP = XMMatrixMultiply(Camera::Instance()->GetViewProj(), mWorld);
	m_D3D->GetDeviceContext()->UpdateSubresource(m_ModelShader->m_MatrixBuffer, 0, NULL, &cb, 0, 0);

	// Render to position textures
	// Set the vertex shader
	//m_D3D->GetDeviceContext()->VSSetShader(m_ModelShader->GetVertexShader(), NULL, 0);
	//m_D3D->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ModelShader->m_MatrixBuffer);

	// Set the pixel shader
	//m_D3D->GetDeviceContext()->PSSetShader(m_ModelShader->GetPixelShader(), NULL, 0);

	// Back-face culling
	m_D3D->GetDeviceContext()->RSSetState(m_D3D->m_backFaceCull);
	//m_D3D->GetDeviceContext()->ClearRenderTargetView(m_ModelBack->m_RTV, ClearRenderTarget);
	//m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_ModelBack->m_RTV, NULL);
	//m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);		// Draw back faces

	// Front-face culling
	//m_D3D->GetDeviceContext()->RSSetState(m_D3D->m_FrontFaceCull);
	//m_D3D->GetDeviceContext()->ClearRenderTargetView(m_ModelFront->m_RTV, ClearRenderTarget);
	//m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_ModelFront->m_RTV, NULL);
	//m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);		// Draw front faces

	// Ray-casting

	// Turn on the alpha blending.
	m_D3D->GetDeviceContext()->OMSetBlendState(m_D3D->m_AlphaState, nullptr, 0xffffffff);

	// Set the input layout
	//m_D3D->GetDeviceContext()->IASetInputLayout(m_ModelShader->GetInputLayout());

	// Render to standard render target
	//m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_D3D->m_renderTargetView, NULL);

	// Set the vertex shader
	m_D3D->GetDeviceContext()->VSSetShader(m_VolumeRaycastShader->GetVertexShader(), NULL, 0);
	m_D3D->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ModelShader->m_MatrixBuffer);

	// Set the pixel shader
	m_D3D->GetDeviceContext()->PSSetShader(m_VolumeRaycastShader->GetPixelShader(), NULL, 0);
	ID3D11Buffer *const Buffers[4] = { m_VolumeRaycastShader->m_WindowSizeCB, m_CamBuffer, m_ObjectBuffer, m_FluidBuffer };
	m_D3D->GetDeviceContext()->PSSetConstantBuffers(0, 4, Buffers);

	// Set texture sampler
	m_D3D->GetDeviceContext()->PSSetSamplers(0, 1, &g_pSamplerLinear);

	// Set textures
	m_D3D->GetDeviceContext()->PSSetShaderResources(0, 1, &m_VolumeTexture->m_fluidShader->m_DensitySRV[0]);//
	m_D3D->GetDeviceContext()->PSSetShaderResources(1, 1, &m_ModelFront->m_SRV);
	m_D3D->GetDeviceContext()->PSSetShaderResources(2, 1, &m_ModelBack->m_SRV);

	// Draw the cube
	m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);

	 //Un-bind textures
	ID3D11ShaderResourceView *nullRV[3] = { NULL, NULL,NULL };
	m_D3D->GetDeviceContext()->PSSetShaderResources(0, 3, nullRV);
}