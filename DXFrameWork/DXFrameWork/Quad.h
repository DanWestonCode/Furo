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

	virtual bool Initialize(ID3D11Device*);
	virtual void Shutdown();
	virtual void Render(ID3D11DeviceContext*);
};

#endif