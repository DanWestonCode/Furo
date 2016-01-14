/*******************************************************************/
/* RenderTexture class encapsulates DX RenderTexture capabilities. */
/* This class is based on DX11 RasterTek tut22 RenderTextureClass  */
/*                                                                 */
/* Created by Daniel Weston 02/01/2016                             */
/*******************************************************************/

#ifndef RenderTexture_h__
#define RenderTexture_h__

#include <d3d11.h>

class RenderTexture
{
public:
	RenderTexture();
	RenderTexture(const RenderTexture&);
	~RenderTexture();

	HRESULT Initialize(ID3D11Device*, int, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
};
#endif // RenderTexture_h__
