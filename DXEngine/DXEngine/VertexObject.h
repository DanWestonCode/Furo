#ifndef VertexObject_h__
#define VertexObject_h__

#include "Object.h"
#include <windows.h>
__declspec(align(16)) class VertexObject : public Object
{
protected:
	struct ModelVertexLayout
	{
		XMFLOAT3 position;
	};
public:
	VertexObject();
	~VertexObject();

	virtual HRESULT Initialise(){ return S_OK; };
	virtual void Render(ID3D11DeviceContext*, XMMATRIX*, XMMATRIX*, XMMATRIX*){};
	virtual void Shutdown();
	virtual void Update(float dt){ Object::Update(dt); };

	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;

protected:
	HRESULT BuildDynamicVB(ID3D11Device*, int, void*);
	HRESULT BuildStaticVB(ID3D11Device*, int, void*);
	HRESULT BuildIndexBuffer(ID3D11Device*, void*);

	XMFLOAT3* vertices;
	WORD indices;

	int m_VertexCount, m_IndexCount;
};
#endif // VertexObject_h__
