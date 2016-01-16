#ifndef Object_h__
#define Object_h__

#include <d3d11.h>
#include <DirectXMath.h>
#include "InputManager.h"
using namespace DirectX;

__declspec(align(16)) class Object
{
public:
	Object();
	~Object();

	void* operator new(size_t);
	void operator delete(void*);

	virtual HRESULT Initialise(){ return S_OK; };
	virtual void Render(ID3D11DeviceContext*, XMMATRIX*, XMMATRIX*, XMMATRIX*){};
	virtual void Shutdown(){};
	virtual void Update(float);

public:
	XMFLOAT3 m_pos, m_rot, m_scale;
	float vel;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_rotationMatrix;

};
#endif // Object_h__
