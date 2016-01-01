/***************************************/
/* The Bitmap class handles the drawn- */
/* of 2D images to the game view       */
/*									   */
/* This class is built from the Bitmap */
/* class presented in the Rastertek    */
/* Tutorials                           */
/* Created by Daniel Weston 23/12/2015 */
/***************************************/
#ifndef Bitmap_h__
#define Bitmap_h__

#include <d3d11.h>
#include <SimpleMath.h>
#include "VertexModel.h"
#include "textureshader.h"

class Bitmap: public VertexModel
{
public:
	Bitmap();
	Bitmap(const Bitmap&);
	~Bitmap();

	virtual HRESULT Initialize(ID3D11Device*, int, int, WCHAR*, int, int, HWND);
	virtual void Shutdown();
	virtual void Render(ID3D11DeviceContext*, int, int, XMMATRIX*, XMMATRIX*, XMMATRIX*);

	void InititalizeBuffers(ID3D11Device*);
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
private:
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
	TextureShader* m_TextureShader;
};
#endif // Bitmap_h__
