#ifndef FluidShader_h__
#define FluidShader_h__

#include "ShaderBase.h"
#include "RenderTexture.h"

__declspec(align(16)) class FluidShader : public ShaderBase
{
protected:
	void CompileShaders(ID3D11Device*);
	void CreateTextures(ID3D11Device*);

protected:
	struct AdvectionBuffer
	{
		float dissipation;
		float dt;
		float decay;
		float padding;
	};
	struct ImpulseBuffer
	{
		float radius;
		float amount;
		float dt;
		XMFLOAT3 sourcePos;
		XMFLOAT3 padding1;
		XMFLOAT3 padding2;
	};
	struct BuoyancyBuffer
	{
		float ambientTemperature;
		float buoyancy;
		float dt;
		float weight;
	};

private:
	void ComputeBoundaryConditions(ID3D11DeviceContext*);
	void ComputeAdvection(ID3D11DeviceContext*, ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*);
	void ComputeImpulse(ID3D11DeviceContext*, ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*);	
	void ComputeBuoyancy(ID3D11DeviceContext*);
	void ComputeVorticityConfinement(ID3D11DeviceContext*);
	void ComputeDivergence(ID3D11DeviceContext*);
	void ComputeJacobi(ID3D11DeviceContext*);

public:
	FluidShader();
	FluidShader(const FluidShader&);
	~FluidShader();

	virtual HRESULT Initialize(ID3D11Device*, ID3D11DeviceContext*, int);
	virtual void Shutdown();
	void Update(ID3D11DeviceContext*, float);

	int size;
	float impulseRadius,
		densityAmount,
		m_timeStep,
		m_dissipation,
		m_decay,
		m_ambientTemperature, 
		m_buoyancy,
		m_weight;
public:
	//shaders
	ID3D11ComputeShader* m_BoundaryConditionsCS;
	ID3D11ComputeShader* m_AdvectionCS;
	ID3D11ComputeShader* m_BuoyancyCS;
	ID3D11ComputeShader* m_ImpulseCS;
	ID3D11ComputeShader* m_VortConCS;
	ID3D11ComputeShader* m_DivergenceCS;
	ID3D11ComputeShader* m_JacobiCS;

	//Textures
	ID3D11Texture3D* m_BoundaryConditions;
	ID3D11Texture3D* m_Velocity[2];
	ID3D11Texture3D* m_Density[2];
	ID3D11Texture3D* m_Vorticity[2];
	ID3D11Texture3D* m_Temperature[2];
	ID3D11Texture3D* m_Divergence;
	ID3D11Texture3D* m_Pressure[2];

	//UAVs
	ID3D11UnorderedAccessView* m_BoundaryConditionsUAV;
	ID3D11UnorderedAccessView* m_VelocityUAV[2];
	ID3D11UnorderedAccessView* m_DensityUAV[2];
	ID3D11UnorderedAccessView* m_VorticityUAV[2];
	ID3D11UnorderedAccessView* m_TemperatureUAV[2];
	ID3D11UnorderedAccessView* m_DivergenceUAV;
	ID3D11UnorderedAccessView* m_PressureUAV[2];

	//SRVs
	ID3D11ShaderResourceView* m_BoundaryConditionsSRV;
	ID3D11ShaderResourceView* m_VelocitySRV[2];
	ID3D11ShaderResourceView* m_DensitySRV[2];
	ID3D11ShaderResourceView* m_VorticitySRV[2];
	ID3D11ShaderResourceView* m_TemperatureSRV[2];
	ID3D11ShaderResourceView* m_DivergenceSRV;
	ID3D11ShaderResourceView* m_PressureSRV[2];

	//buffer
	ID3D11Buffer* m_DensityBuffer;
	ID3D11Buffer* m_AdvectionBuffer;
	ID3D11Buffer* m_BuoyancyBuffer;
};
#endif // FluidShader_h__
