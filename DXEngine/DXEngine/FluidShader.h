#ifndef FluidShader_h__
#define FluidShader_h__

#include "ShaderBase.h"
#include "RenderTexture.h"

__declspec(align(16)) class FluidShader : public ShaderBase
{
protected:
	void CompileShaders(ID3D11Device*);
	void CreateBuffers(ID3D11Device*);

protected:
	struct BoundaryConditions
	{
		XMFLOAT3 x;
		XMFLOAT3 y;
	};

public:
	FluidShader();
	FluidShader(const FluidShader&);
	~FluidShader();

	virtual HRESULT Initialize(ID3D11Device*);
	virtual void Shutdown();
	void Render(ID3D11DeviceContext*);

public:
	//shaders
	ID3D11ComputeShader* m_BoundaryConditionsCS;

	//buffers
	ID3D11Buffer* m_outputBuffer;
	ID3D11Buffer* m_outputresult;

	//UAVS
	ID3D11UnorderedAccessView* m_BoundaryConditionsUAV;

	//vars
	BoundaryConditions* m_BoundaryConditionsWRITE;
	BoundaryConditions*	m_BoundaryConditionsREAD;
};
#endif // FluidShader_h__
