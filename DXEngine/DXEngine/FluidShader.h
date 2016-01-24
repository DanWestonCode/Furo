#ifndef FluidShader_h__
#define FluidShader_h__

#include "ShaderBase.h"
#include "RenderTexture.h"

__declspec(align(16)) class FluidShader : public ShaderBase
{
protected:
	struct MatrixBuffer
	{
		XMMATRIX mWVP;
	};

public:
	FluidShader();
	FluidShader(const FluidShader&);
	~FluidShader();

	virtual HRESULT Initialize(ID3D11Device*, HWND);
	virtual void Shutdown();
	void Render(ID3D11DeviceContext*, XMMATRIX*, int, RenderTexture*, RenderTexture*, ID3D11RenderTargetView*);

	ID3D11Buffer* m_MatrixBuffer;
	};
#endif // FluidShader_h__
