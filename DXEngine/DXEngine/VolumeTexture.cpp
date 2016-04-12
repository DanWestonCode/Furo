/// <summary>
/// VolumeTexture.h
///
/// About:
/// VolumeTexture.h loads a RAW file into a ID3D11Texture3D object 
/// The implementation was guided by Dr.Christian B Mendl adaptation 
/// of volume rendering 101's implementation of the functionality
///
/// Dr.Christian B Mendl: http://christian.mendl.net/pages/software.html - See 'InitDevice' function in 'Volume Ray-Casting Renderer project'
/// Volume Rendering 101: http://graphicsrunner.blogspot.co.uk/2009/01/volume-rendering-101.html
/// </summary>
#include "VolumeTexture.h"
#include "InputManager.h"
VolumeTexture::VolumeTexture()
{
	m_Texture3D = nullptr;
	m_ShaderResourceView = nullptr;
}
VolumeTexture::VolumeTexture(const VolumeTexture& other){}
VolumeTexture::~VolumeTexture(){}

HRESULT VolumeTexture::Initialize(D3D* _d3d, int _volSize)
{
	HRESULT result = S_OK;

	m_CPUFluid3D = new StamSolver3D;
	m_CPUFluid3D->Initialize(_volSize);

	BYTE *buffer = (BYTE *)malloc(_volSize*_volSize*_volSize * sizeof(BYTE));
	D3D11_TEXTURE3D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.Height = _volSize;
	descTex.Width = _volSize;
	descTex.Depth = _volSize;
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R8_UNORM;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_SHADER_RESOURCE;
	descTex.CPUAccessFlags = 0;
	// Initial data
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = buffer;
	initData.SysMemPitch = _volSize;
	initData.SysMemSlicePitch = _volSize*_volSize;
	// Create texture
	result = (_d3d->GetDevice()->CreateTexture3D(&descTex, &initData, &m_Texture3D));
	// Create a resource view of the texture
	result = (_d3d->GetDevice()->CreateShaderResourceView(m_Texture3D, NULL, &m_ShaderResourceView));

	free(buffer);

	return result;
}

void VolumeTexture::Shutdown()
{
	m_Texture3D->Release();
	m_Texture3D = nullptr;
	m_ShaderResourceView->Release();
	m_ShaderResourceView = nullptr;
}

void VolumeTexture::Update(ID3D11Device* _device, int _volSize, float dt)
{
	m_CPUFluid3D->Run(0.1f);
	m_CPUFluid3D->Clear();

	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			for (int z = 0; z < 2; z++)
			{
				m_CPUFluid3D->SetDensity(x, y, z, 1);
				m_CPUFluid3D->SetVelX(x, y, z, 1);
				m_CPUFluid3D->SetVelY(x, y, z, 1);
				m_CPUFluid3D->SetVelZ(x, y, z, 1);
			}
		}
	}


	BYTE *buffer = (BYTE *)malloc(_volSize*_volSize*_volSize * sizeof(BYTE));
	buffer = (BYTE *)m_CPUFluid3D->m_density;


	D3D11_TEXTURE3D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.Height = _volSize;
	descTex.Width = _volSize;
	descTex.Depth = _volSize;
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R8_UNORM;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_SHADER_RESOURCE;
	descTex.CPUAccessFlags = 0;
	// Initial data
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = buffer;
	initData.SysMemPitch = _volSize;
	initData.SysMemSlicePitch = _volSize*_volSize;
	// Create texture
	(_device->CreateTexture3D(&descTex, &initData, &m_Texture3D));

	// Create a resource view of the texture
	(_device->CreateShaderResourceView(m_Texture3D, NULL, &m_ShaderResourceView));
}