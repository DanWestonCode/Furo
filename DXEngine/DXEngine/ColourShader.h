#ifndef TextureShader_h__
#define TextureShader_h__

#include "ShaderBase.h"

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
	void Render(ID3D11DeviceContext*, XMMATRIX*, int);

	ID3D11Buffer* m_MatrixBuffer;
};
#endif // TextureShader_h__
