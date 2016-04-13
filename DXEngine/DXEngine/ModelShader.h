/// <summary>
/// ModelShader.h
///
/// About:
/// ModelShader.h interfaces the model_position.hlsl shader and derives 
/// from the shader base class. 
/// </summary>

#ifndef ModelShader_h__
#define ModelShader_h__

#include "ShaderBase.h"

__declspec(align(16)) class ModelShader : public ShaderBase
{
	struct MatrixBuffer
	{
		XMMATRIX mWVP;
	};
public:
	ModelShader();
	ModelShader(const ModelShader&);
	~ModelShader();

	HRESULT Initialize(ID3D11Device*, HWND);
	void Shutdown();

	ID3D11Buffer* m_MatrixBuffer;
};
#endif // ModelShader_h__
