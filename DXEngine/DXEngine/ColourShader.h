#ifndef ColourShader_h__
#define ColourShader_h__

#include "ShaderBase.h"
#include "RenderTexture.h"

__declspec(align(16)) class ColourShader : public ShaderBase
{
protected:
	struct MatrixBuffer
	{
		XMMATRIX mWVP;
	};
public:
	ColourShader();
	ColourShader(const ColourShader&);
	~ColourShader();	

	virtual HRESULT Initialize(ID3D11Device*, HWND);
	virtual void Shutdown();
	void Render(ID3D11DeviceContext*, XMMATRIX*, int, RenderTexture*);

	ID3D11Buffer* m_MatrixBuffer;
};
#endif // ColourShader_h__
