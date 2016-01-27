#include "VolumeTexture.h"
#include "InputManager.h"
VolumeTexture::VolumeTexture()
{
	m_Texture3D = nullptr;
	m_ShaderResourceView = nullptr;
	m_furo = nullptr;
}
VolumeTexture::VolumeTexture(const VolumeTexture& other){}
VolumeTexture::~VolumeTexture(){}

HRESULT VolumeTexture::Initialize(ID3D11Device* _device, int _volSize)
{
	m_furo = new Furo;
	m_furo->Initialize(Furo::ThreeDimensional, _volSize, 0);

	HRESULT result = S_OK;
	HANDLE hFile = CreateFileW(L"../DXEngine/foot.raw", GENERIC_READ, 0, NULL, OPEN_EXISTING, OPEN_EXISTING, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, L"Opening volume data file failed.", L"Error", MB_ICONERROR | MB_OK);
	}

	BYTE *buffer = (BYTE *)malloc(_volSize*_volSize*_volSize * sizeof(BYTE));


	DWORD numberOfBytesRead = 0;
	if (ReadFile(hFile, buffer, _volSize*_volSize*_volSize, &numberOfBytesRead, NULL) == 0)
	{
		MessageBox(NULL, L"Reading volume data failed.", L"Error", MB_ICONERROR | MB_OK);
	}

	CloseHandle(hFile);


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
	result = (_device->CreateTexture3D(&descTex, &initData, &m_Texture3D));

	// Create a resource view of the texture
	result = (_device->CreateShaderResourceView(m_Texture3D, NULL, &m_ShaderResourceView));

	free(buffer);

	return result;
}

void VolumeTexture::Update(ID3D11Device* _device, int _volSize, float dt)
{
	m_furo->Run(dt);
	m_furo->m_volumeFluid->Clear();
	if (InputManager::Instance()->IsKeyDown(DIK_Q))
	{
		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int z = 0; z < 2; z++)
				{
					m_furo->m_volumeFluid->SetDensity(x, y, z, 1);
					m_furo->m_volumeFluid->SetVelX(x, y, z, 1);
					m_furo->m_volumeFluid->SetVelY(x, y, z, 1);
					m_furo->m_volumeFluid->SetVelZ(x, y, z, 1);
				}
			}
		}
	}

	

	BYTE *buffer = (BYTE *)malloc(_volSize*_volSize*_volSize * sizeof(BYTE));
	buffer = (BYTE *)m_furo->m_volumeFluid->GetDensity();
		

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

void VolumeTexture::Shutdown()
{
	m_Texture3D->Release();
	m_Texture3D = nullptr;
	m_ShaderResourceView->Release();
	m_ShaderResourceView = nullptr;

	m_furo->Shutdown();
}