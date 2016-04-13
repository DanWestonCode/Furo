/// <summary>
/// Quad.h
///
/// About:
/// Quad.h derives from VertexObject. The class creates a Quad VO object.
/// Within the Quad class the implementation for the 
/// 2D CPU Fluid is also found
///</summary>
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
	struct SimProps
	{
		float veloMulti;
		float densityMulti;
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
	SimProps m_props;
};

#endif // Quad_h__
