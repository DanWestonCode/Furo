/*******************************************************************/
/* Object is the base class for all GameObject based classes.      */
/* The Object class contains several functions and variables       */
/* which can be overridden by inherited classes                    */
/*									                               */
/* Created by Daniel Weston 01/01/2016                             */
/*******************************************************************/

#ifndef Object_h__
#define Object_h__

#include "InputManager.h"
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//Fix for 'warning C4316: object allocated on the heap may not be aligned 16'
__declspec(align(16)) class Object

{
public:
	Object();
	~Object();

	void* operator new(size_t);
	void operator delete(void*);

	virtual HRESULT Initialise(){ return S_OK; };
	virtual void Render(ID3D11DeviceContext*, XMMATRIX*, XMMATRIX*, XMMATRIX* ){};
	virtual void Shutdown(){};
	virtual void Update(float);

protected:
	Vector3 m_pos, m_rot, m_scale;
	float vel;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_rotationMatrix;
};
#endif // GameObject_h__
