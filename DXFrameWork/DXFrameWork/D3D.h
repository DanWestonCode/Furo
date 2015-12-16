#ifndef _D3D_H_
#define  _D3D_H_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <SimpleMath.h>
#include <d3d11.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class D3D
{
public:
	D3D();
	D3D(const D3D&);
	~D3D();
	 

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(Color);//(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(Matrix&);
	void GetWorldMatrix(Matrix&);
	void GetOrthoMatrix(Matrix&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;

	Matrix m_projectionMatrix;
	Matrix m_worldMatrix;
	Matrix m_orthoMatrix;
};
#endif // !_D3D_H_
