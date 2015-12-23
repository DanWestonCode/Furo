#ifndef _QUAD_H_
#define _QUAD_H_
#include "VertexModel.h"

using namespace DirectX;
class Quad : public VertexModel
{
public:
	Quad();
	Quad(const Quad&);
	~Quad();

	virtual HRESULT Initialize(ID3D11Device*, WCHAR*);
	virtual void Shutdown();
	virtual void Render(ID3D11DeviceContext*);

	void UpdateTexture(float*);

	int numTris;
};

#endif