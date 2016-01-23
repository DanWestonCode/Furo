#ifndef Quad_h__
#define Quad_h__

#include "VertexObject.h"
#include "ColourShader.h"
#include "Furo.h"


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

	virtual HRESULT Initialise(ID3D11Device*, HWND);
	virtual void Render(ID3D11DeviceContext*);
	virtual void Shutdown();
	virtual void Update(float);

private:
	void UpdateFluid(float* dens);
	int numTris;

public:
	ColourShader* m_ColorShader;
	ColorVL* m_ColorVertLayout;
	Furo* m_Furo;
};

#endif // Quad_h__
