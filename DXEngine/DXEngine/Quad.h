#ifndef Quad_h__
#define Quad_h__

#include "VertexObject.h"
#include "ColourShader.h"
#include "FluidShader.h"
#include "RenderTexture.h"
#include "Furo.h"
#include "D3D.h"

__declspec(align(16)) class Quad : public VertexObject
{
protected:
	struct ColorVL
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public: 
	Quad();
	Quad(const Quad&);
	~Quad();

	virtual HRESULT Initialise(D3D*, HWND);
	virtual void Render(D3D*);
	virtual void Shutdown();
	virtual void Update(float, HWND);


private:
	void UpdateFluid(float* dens);
	int numTris;
	unsigned long* indices;
public:
	RenderTexture* m_TextureB;
	RenderTexture* m_TextureA;
	ColourShader* m_ColorShader;
	FluidShader* m_FluidShader;
	ColorVL* m_ColorVertLayout;
	Furo* m_Furo;
protected:
	float veloMulti;
	float densityMulti;
};

#endif // Quad_h__
