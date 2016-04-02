#ifndef Quad_h__
#define Quad_h__

#include "VertexObject.h"
#include "ColourShader.h"
#include "StamSolver2D.h"
#include "D3D.h"
using namespace Furo;

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

	HRESULT Initialise(D3D*, HWND);
	void Render(D3D*);
	void Shutdown();
	void Update(float, HWND);


private:
	void UpdateFluid(float* dens);
	int numTris;
	unsigned long* indices;
public:
	ColourShader* m_ColorShader;
	ColorVL* m_ColorVertLayout;

	StamSolver2D* m_fluid;
protected:
	float veloMulti;
	float densityMulti;
};

#endif // Quad_h__
