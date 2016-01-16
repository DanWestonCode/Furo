#ifndef Cube_h__
#define Cube_h__

#include "VertexObject.h"

__declspec(align(16)) class Cube : public VertexObject
{
public:
	Cube();
	~Cube();

	virtual HRESULT Initialise(ID3D11Device*);
	virtual void Render(ID3D11DeviceContext*);
	virtual void Shutdown();
	virtual void Update(float);

public:
	ID3D11Buffer* m_IndexBuffer;
	ID3D11Buffer* m_VertexBuffer;
};
#endif // Cube_h__
