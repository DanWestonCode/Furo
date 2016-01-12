/*******************************************************************/
/* The Cube Class is a vertex type object, hence its parent class. */
/* The Cube class sets up its buffers and vertex data in the       */
/* initialize function. The render function handles updating the   */
/* buffers as well as sending its buffer data to the shader        */
/*									                               */
/* Created by Daniel Weston 07/01/2016                             */
/*******************************************************************/
#ifndef Cube_h__
#define Cube_h__
#include "VertexModel.h"
#include "RayCaster.h"

using namespace DirectX;
__declspec(align(16)) class Cube : public VertexModel
{
protected:


public:
	Cube();
	Cube(const Cube&);
	~Cube();

	//inherited functions
	virtual HRESULT Initialize(ID3D11Device*, HWND);
	virtual void Shutdown();
	virtual void Render(ID3D11DeviceContext*, XMMATRIX*, XMMATRIX*, XMMATRIX*);
	virtual void Update(float);
	

	int numTris;

private:
	RayCaster* m_rayCaster;


};

#endif // Cube_h__
