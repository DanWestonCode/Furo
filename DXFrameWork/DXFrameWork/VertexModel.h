#ifndef _MODEL_H_
#define _MODEL_H_

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

	virtual bool Initialize(ID3D11Device*);
	virtual void Shutdown();
	virtual void Render(ID3D11DeviceContext*);

	int GetIndexCount();
private:
	void RenderBuffers(ID3D11DeviceContext*);
protected:
	//Build index and vertex buffers
	HRESULT BuildDynamicVB(ID3D11Device*, int, void*);
	HRESULT BuildStaticVB(ID3D11Device*, int, void*);
	HRESULT BuildIndexBuffer(ID3D11Device*, void*);

	VertexType* vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;	
	int m_vertexCount, m_indexCount;
};

#endif