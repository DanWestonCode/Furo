/***********************************
* ModelShader.h
* Created by Daniel 14 11/01/16
*
* ModelShader encapsulates the 
* ModelShader.hlsl shaders
* *********************************/

#ifndef ModelShader_h__
#define ModelShader_h__

#include "ShaderBase.h"
#include <DirectXMath.h>

using namespace DirectX;
__declspec(align(16)) class ModelShader : public ShaderBase
{
protected:
	struct MatrixBuffer
	{
		XMMATRIX mWVP;
	};
public:
	ModelShader();
	ModelShader(const ModelShader&);
	~ModelShader();

	virtual HRESULT Initialize(ID3D11Device*, HWND);
	virtual void Shutdown();

	ID3D11Buffer* m_MatrixBuffer;
};
#endif // ModelShader_h__
