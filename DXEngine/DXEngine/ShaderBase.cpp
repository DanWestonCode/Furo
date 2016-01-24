/***********************************
* ShaderBase.cpp
* Created by Daniel 14 11/01/16
*
* ShaderBase is intended as a base
* class for all classes encapsulating
* shaders.
* *********************************/
#include "ShaderBase.h"
#include "Camera.h"
ShaderBase::ShaderBase()
{
	m_PixelShader = nullptr;
	m_VertexShader = nullptr;
	m_Sampler = nullptr;
}
ShaderBase::ShaderBase(const ShaderBase& other){}
ShaderBase::~ShaderBase(){};

void* ShaderBase::operator new(size_t memorySize)
{
	return _aligned_malloc(memorySize, 16);

}

void ShaderBase::operator delete(void* memoryBlockPtr)
{
	_aligned_free(memoryBlockPtr);
	return;
}

HRESULT ShaderBase::CompileShaderFromFile(WCHAR *ShaderFileName, LPCSTR ShaderEntryPoint, LPCSTR ShaderModel, ID3DBlob **ppBlobOut)
{
	HRESULT hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *pErrorBlob;
	hr = D3DCompileFromFile(ShaderFileName, NULL, NULL, ShaderEntryPoint, ShaderModel, dwShaderFlags, 0,
		ppBlobOut, &pErrorBlob);

	return hr;
}

void ShaderBase::Shutdown()
{
	m_VertexShader->Release();
	m_VertexShader = nullptr;
	m_PixelShader->Release();
	m_PixelShader = nullptr;
	m_InputLayout->Release();
	m_InputLayout = nullptr;
	m_Sampler->Release();
	m_Sampler = nullptr;
}

