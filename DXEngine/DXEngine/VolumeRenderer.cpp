/// <summary>
/// VolumeRenderer.h
///
/// About:
/// VolumeRenderer.h handles all the objects required 
/// rendering ID3D11Texture3D's. 
/// Implementation was guided by Dr.Christian B Mendl implementation of the Volume Rendering 101 article
///
/// Dr.Christian B Mendl: http://christian.mendl.net/pages/software.html - See 'Render' function in 'Volume Ray-Casting Renderer project'
/// Volume Rendering 101: http://graphicsrunner.blogspot.co.uk/2009/01/volume-rendering-101.html
/// </summary>

#include "VolumeRenderer.h"
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

//Fix for 'warning C4316: object allocated on the heap may not be aligned 16'
//This kept giving me access violation errors using XMMatrix calculations
//functions discovered from:-
//http://stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16
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

	m_cube = new Cube;
	m_cube->Initialise(_d3d->GetDevice());

	CreateSampler(_d3d->GetDevice());
#pragma region Initial vars
	//set initial vars
	m_RenderProps.iterations = 128;
	m_RenderProps.stepSize = sqrt(3.f) / m_RenderProps.iterations;
	m_RenderProps.fluidCol = XMFLOAT3(0.1f, 1.0f, 1.0f);
	m_RenderProps.absoprtion = 0.05f;
#pragma endregion

#pragma region Constant Buffer
	m_RenderPropsBuffer = nullptr;
	//set prev vars up
	m_PrevProps = m_RenderProps;
	//create constant buffer 
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	//Render props buffer
	bufferDesc.ByteWidth = sizeof(RenderProps);
	result = _d3d->GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_RenderPropsBuffer);
	//map and unmap struct
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	RenderProps* propsBuffer;
	//Props Buffer
	result = _d3d->GetDeviceContext()->Map(m_RenderPropsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	propsBuffer = (RenderProps*)mappedResource.pData;
	propsBuffer->fluidCol = m_RenderProps.fluidCol;
	propsBuffer->iterations = m_RenderProps.iterations;
	propsBuffer->stepSize = m_RenderProps.stepSize;
	propsBuffer->absoprtion = m_RenderProps.absoprtion;
	_d3d->GetDeviceContext()->Unmap(m_RenderPropsBuffer, 0);
#pragma endregion
	
#pragma region AnTweakBar Renderer Vars
	TwStructMember _TwFluidProps[] = {
		{ "Fluid Colour", TW_TYPE_COLOR3F, offsetof(RenderProps, fluidCol), "min=0.01 max=0.08 step=0.01" },
		{ "Iterations", TW_TYPE_INT32, offsetof(RenderProps, iterations), "min=1 max=129 step=1" },
		{ "Absoprtion", TW_TYPE_FLOAT, offsetof(RenderProps, absoprtion), "min=0.05 max=0.60 step=0.01" },
	};

	TwAddVarRW(_d3d->m_TwBar, "Renderer", TwDefineStruct("Renderer", _TwFluidProps, 3, sizeof(RenderProps), nullptr, nullptr), &m_RenderProps, NULL);
#pragma endregion

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
	UpdateBuffers(_d3d);
}

void VolumeRenderer::UpdateBuffers(D3D* _d3d)
{
	HRESULT result;

	//only update step size if iterations have changed
	if (m_PrevProps.iterations != m_RenderProps.iterations)
	{
		m_RenderProps.stepSize = sqrt(3.f) / m_RenderProps.iterations;
	}

#pragma region Update Render Properties
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	RenderProps* propsBuffer;
	//Props Buffer
	result = _d3d->GetDeviceContext()->Map(m_RenderPropsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	propsBuffer = (RenderProps*)mappedResource.pData;
	propsBuffer->fluidCol = m_RenderProps.fluidCol;
	
	propsBuffer->fluidCol = m_RenderProps.fluidCol;
	propsBuffer->iterations = m_RenderProps.iterations;
	propsBuffer->stepSize = m_RenderProps.stepSize;
	propsBuffer->absoprtion = m_RenderProps.absoprtion;
	_d3d->GetDeviceContext()->Unmap(m_RenderPropsBuffer, 0);
#pragma endregion
	m_PrevProps = m_RenderProps;
}

void VolumeRenderer::Render(D3D* m_D3D, ID3D11ShaderResourceView* _vol)
{
	float ClearRenderTarget[4] = { 0.f, 0.f, 0.f, 1.f };

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
	m_D3D->GetDeviceContext()->VSSetShader(m_ModelShader->GetVertexShader(), NULL, 0);
	m_D3D->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ModelShader->m_MatrixBuffer);

	// Set the pixel shader
	m_D3D->GetDeviceContext()->PSSetShader(m_ModelShader->GetPixelShader(), NULL, 0);

	// Back-face culling
	m_D3D->GetDeviceContext()->RSSetState(m_D3D->m_backFaceCull);
	m_D3D->GetDeviceContext()->ClearRenderTargetView(m_ModelBack->m_RTV, ClearRenderTarget);
	m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_ModelBack->m_RTV, NULL);
	m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);		// Draw back faces

	// Front-face culling
	m_D3D->GetDeviceContext()->RSSetState(m_D3D->m_FrontFaceCull);
	m_D3D->GetDeviceContext()->ClearRenderTargetView(m_ModelFront->m_RTV, ClearRenderTarget);
	m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_ModelFront->m_RTV, NULL);
	m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);		// Draw front faces

	// Ray-casting

	// Turn on the alpha blending.
	m_D3D->GetDeviceContext()->OMSetBlendState(m_D3D->m_AlphaState, nullptr, 0xffffffff);

	// Set the input layout
	m_D3D->GetDeviceContext()->IASetInputLayout(m_ModelShader->GetInputLayout());

	//Render to standard render target
	m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &m_D3D->m_renderTargetView, NULL);

	// Set the vertex shader
	m_D3D->GetDeviceContext()->VSSetShader(m_VolumeRaycastShader->GetVertexShader(), NULL, 0);
	m_D3D->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_ModelShader->m_MatrixBuffer);

	// Set the pixel shader
	m_D3D->GetDeviceContext()->PSSetShader(m_VolumeRaycastShader->GetPixelShader(), NULL, 0);
	ID3D11Buffer *const Buffers[2] = { m_VolumeRaycastShader->m_WindowSizeCB, m_RenderPropsBuffer};
	m_D3D->GetDeviceContext()->PSSetConstantBuffers(0, 2, Buffers);

	// Set texture sampler
	m_D3D->GetDeviceContext()->PSSetSamplers(0, 1, &g_pSamplerLinear);

	// Set textures
	m_D3D->GetDeviceContext()->PSSetShaderResources(0, 1, &_vol);//
	m_D3D->GetDeviceContext()->PSSetShaderResources(1, 1, &m_ModelFront->m_SRV);
	m_D3D->GetDeviceContext()->PSSetShaderResources(2, 1, &m_ModelBack->m_SRV);

	// Draw the cube
	m_D3D->GetDeviceContext()->DrawIndexed(36, 0, 0);

	 //Un-bind textures
	ID3D11ShaderResourceView *nullRV[3] = { NULL, NULL,NULL };
	m_D3D->GetDeviceContext()->PSSetShaderResources(0, 3, nullRV);
}