#ifndef RenderTexture_h__
#define RenderTexture_h__
#include <d3d11.h>
#include <string>


class RenderTexture
{
public:
	RenderTexture();
	RenderTexture(const RenderTexture&);
	~RenderTexture();

	HRESULT Initialize(ID3D11Device*, int, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*);
public:
	std::string  tag;
	ID3D11Texture2D* m_Texture2D;
	ID3D11ShaderResourceView* m_ShaderResourceView;
	ID3D11RenderTargetView*	m_RenderTargetView;
};
#endif // RenderTexture_h__
