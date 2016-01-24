/***********************************
* ShaderBase.h
* Created by Daniel 14 11/01/16
*
* ShaderBase is intended as a base 
* class for all classes encapsulating
* shaders.
* *********************************/
#ifndef ShaderBase_h__
#define ShaderBase_h__

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Camera.h"

using namespace DirectX;
__declspec(align(16)) class ShaderBase
{
public:
	ShaderBase();
	ShaderBase(const ShaderBase&);
	~ShaderBase();

	void* operator new(size_t);
	void operator delete(void*);

	virtual HRESULT Initialize(ID3D11Device*, HWND) { return S_OK; };
	virtual void Shutdown();

	//Shader compilation helper function - Christian B. Mendl, 2012 - editied by Daniel Weston 
	HRESULT CompileShaderFromFile(WCHAR *, LPCSTR, LPCSTR, ID3DBlob **);

	//Getters for VS, PS and InputLayout
	ID3D11VertexShader* GetVertexShader(){ return m_VertexShader; }
	ID3D11PixelShader* GetPixelShader(){ return m_PixelShader; }
	ID3D11InputLayout* GetInputLayout(){ return m_InputLayout; }

protected:

	ID3D11VertexShader* m_VertexShader;
	ID3D11InputLayout* m_InputLayout;
	ID3D11PixelShader* m_PixelShader;
	ID3D11SamplerState* m_Sampler;
};
#endif // ShaderBase_h__
