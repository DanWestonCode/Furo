#ifndef FluidShader_h__
#define FluidShader_h__

#include "ShaderBase.h"
#include "RenderTexture.h"

__declspec(align(16)) class FluidShader : public ShaderBase
{
protected:
	void CompileShaders(ID3D11Device*);
	void CreateBuffers(ID3D11Device*);

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

	//Textures
	ID3D11Texture3D* m_BoundaryConditions;

	//UAVs
	ID3D11UnorderedAccessView* m_BoundaryConditionsUAV;

	//SRVs
	ID3D11ShaderResourceView* m_BoundaryConditionsSRV;
};
#endif // FluidShader_h__
