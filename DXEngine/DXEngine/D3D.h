/*******************************************************************/
/* The D3D class encapsulates the set up stages for creating a     */
/* DirectX Device. This class is based from RasterTek tutorials    */
/*                                                                 */
/* Created by Daniel Weston 21/12/2015                             */
/*******************************************************************/

#ifndef _D3D_H_
#define  _D3D_H_

//Specify libraries for linking
#pragma comment(lib, "dxgi.lib") //D3D functionality
#pragma comment(lib, "d3d11.lib") //Tools to interface the hardwares
#pragma comment(lib, "d3dcompiler.lib") //used for compiling shaders

#include <d3d11.h>
#include <DirectXMath.h>
#include "AntTweakBar.h"

using namespace DirectX;

__declspec(align(16)) class D3D
{
public:
	D3D();
	D3D(const D3D&);
	~D3D();


	void* operator new(size_t);
	void operator delete(void*);

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void SetRasterState(ID3D11RasterizerState* state);

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();

	ID3D11RasterizerState* m_backFaceCull;
	ID3D11RasterizerState* m_FrontFaceCull;

	ID3D11RenderTargetView* m_renderTargetView;
	TwBar* m_TwBar;

	int m_ScreenWidth, m_ScreenHeight;
	
private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;

	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;


	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;

	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

	ID3D11DepthStencilState* m_depthDisabledStencilState;
	
	
};
#endif // !_D3D_H_
