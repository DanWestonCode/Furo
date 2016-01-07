/*******************************************************************/
/* The Quad Class is a vertex type object, hence its parent class. */ 
/* The Quad class sets up its buffers and vertex data in the       */
/* initialize function. The render function handles updating the   */
/* buffers as well as sending its buffer data to the shader        */
/*									                               */                       
/* Created by Daniel Weston 21/12/2015                             */
/*******************************************************************/

#ifndef _QUAD_H_
#define _QUAD_H_
#include "VertexModel.h"


using namespace DirectX;
__declspec(align(16)) class Quad : public VertexModel
{
protected:
public:
	Quad();
	Quad(const Quad&);
	~Quad();

	//inherited functions
	virtual HRESULT Initialize(ID3D11Device*, WCHAR*, HWND);
	virtual void Shutdown();
	virtual void Render(ID3D11DeviceContext*, XMMATRIX*, XMMATRIX*, XMMATRIX*);
	virtual void Update(float);

	void UpdateTexture(float*);

	int numTris;	
};

#endif