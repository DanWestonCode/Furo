/// <summary>
/// ColourShader.h
///
/// About:
/// ColourShader.h interfaces the Colour.hlsl shader and derives 
/// from the shader base class. The implementation iss based from 
/// the ColorShaderClass found in RasterTek tutorials
///
/// Based from:
/// ColourShaderClass.h - http://www.rastertek.com/dx11tut04.html
/// </summary>
#ifndef ColourShader_h__
#define ColourShader_h__

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

	HRESULT Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void Render(ID3D11DeviceContext*, XMMATRIX*, int);

	ID3D11Buffer* m_MatrixBuffer;
};
#endif // ColourShader_h__