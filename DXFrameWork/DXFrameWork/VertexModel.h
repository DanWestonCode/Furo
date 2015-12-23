#ifndef _MODEL_H_
#define _MODEL_H_

#include "TextureLoader.h"
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class VertexModel
{
protected:
	struct VertexType
	{
		Vector3 position;
		Color color;
		Vector2 texture;
	};

	enum VertexBufferType
	{
		_static,
		_dynamic
	};
public:
	VertexModel();
	VertexModel(const VertexModel&);
	~VertexModel();

	virtual HRESULT Initialize(ID3D11Device*, WCHAR*);
	virtual void Shutdown();
	virtual void Render(ID3D11DeviceContext*);
	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();
private:
	void RenderBuffers(ID3D11DeviceContext*);

	
	
protected:
	//Build index and vertex buffers
	HRESULT BuildDynamicVB(ID3D11Device*, int, void*);
	HRESULT BuildStaticVB(ID3D11Device*, int, void*);
	HRESULT BuildIndexBuffer(ID3D11Device*, void*);
	HRESULT LoadTexture(ID3D11Device*, WCHAR*);

	VertexType* vertices;
	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;
	ID3D11ShaderResourceView* m_Texture;
	int m_VertexCount, m_IndexCount;

	void ReleaseTexture();	
};

#endif