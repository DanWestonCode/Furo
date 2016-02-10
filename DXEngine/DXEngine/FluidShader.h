#ifndef FluidShader_h__
#define FluidShader_h__

#include "ShaderBase.h"
#include "RenderTexture.h"

__declspec(align(16)) class FluidShader : public ShaderBase
{
protected:
	void CompileShaders(ID3D11Device*);
	void CreateTextures(ID3D11Device*);

public:
	FluidShader();
	FluidShader(const FluidShader&);
	~FluidShader();

	virtual HRESULT Initialize(ID3D11Device*);
	virtual void Shutdown();
	void Render(ID3D11DeviceContext*);

private:
	void ComputeBoundaryConditions(ID3D11DeviceContext*);
	void ComputeUnbind(ID3D11DeviceContext*, int);

public:
	//shaders
	ID3D11ComputeShader* m_BoundaryConditionsCS;

	//Textures
	ID3D11Texture3D* m_BoundaryConditions;
	ID3D11Texture3D* m_Velocity[2];
	ID3D11Texture3D* m_Density[2];
	ID3D11Texture3D* m_Temperature[2];
	ID3D11Texture3D* m_Vorticity[2];

	//UAVs
	ID3D11UnorderedAccessView* m_BoundaryConditionsUAV;
	ID3D11UnorderedAccessView* m_VelocityUAV[2];
	ID3D11UnorderedAccessView* m_DensityUAV[2];
	ID3D11UnorderedAccessView* m_TemperatureUAV[2];
	ID3D11UnorderedAccessView* m_VorticityUAV[2];

	//SRVs
	ID3D11ShaderResourceView* m_BoundaryConditionsSRV;
	ID3D11ShaderResourceView* m_VelocitySRV[2];
	ID3D11ShaderResourceView* m_DensitySRV[2];
	ID3D11ShaderResourceView* m_TemperatureSRV[2];
	ID3D11ShaderResourceView* m_VorticitySRV[2];
};
#endif // FluidShader_h__
