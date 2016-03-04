#ifndef FluidShader_h__
#define FluidShader_h__

#include "ShaderBase.h"
#include "RenderTexture.h"
#include "AntTweakBar.h"
#include <stddef.h>

__declspec(align(16)) class FluidShader : public ShaderBase
{
protected:
	void CompileShaders(ID3D11Device*);
	void CreateResources(ID3D11Device*);

protected:
	struct AdvectionBuffer
	{
		float dissipation;
		float dt;
		float decay;
		float forward;
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
	struct ConfinementBuffer
	{
		float dt;
		float VorticityStrength;
		float padding1;
		float padding2;
	};

	struct SimulationVars
	{
		float m_impulseRadius,
			m_densityAmount,
			m_TemperatureAmount,
			m_timeStep,
			m_densityDissipation,
			m_velocityDissipation,
			m_temperatureDissipation,
			m_decay,
			m_ambientTemperature,
			m_buoyancy,
			m_weight,
			m_VorticityStrength;
	};

private:
	void ComputeBoundaryConditions(ID3D11DeviceContext*);
	void ComputeAdvection(ID3D11DeviceContext*, ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*, float, float);
	void ComputeMacCormack(ID3D11DeviceContext*, ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*, float, float);
	void ComputeImpulse(ID3D11DeviceContext*, ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*, float);	
	void ComputeBuoyancy(ID3D11DeviceContext*);
	void ComputeVorticity(ID3D11DeviceContext*);
	void ComputeConfinement(ID3D11DeviceContext*);
	void ComputeDivergence(ID3D11DeviceContext*);
	void ComputeJacobi(ID3D11DeviceContext*);
	void ComputeProjection(ID3D11DeviceContext*);

public:
	FluidShader();
	FluidShader(const FluidShader&);
	~FluidShader();

	virtual HRESULT Initialize(ID3D11Device*, ID3D11DeviceContext*, TwBar*, int);
	virtual void Shutdown();
	void Update(ID3D11DeviceContext*, float);

	int FluidSize;
	float m_timeStep;

public:
	SimulationVars m_simVars;
	TwType smokePropertiesStructMembers;

	//shaders
	ID3D11ComputeShader* m_BoundaryConditionsCS;
	ID3D11ComputeShader* m_AdvectionCS;
	ID3D11ComputeShader* m_MacCormackCS;
	ID3D11ComputeShader* m_BuoyancyCS;
	ID3D11ComputeShader* m_ImpulseCS;
	ID3D11ComputeShader* m_VorticityCS;
	ID3D11ComputeShader* m_ConfinementCS;
	ID3D11ComputeShader* m_DivergenceCS;
	ID3D11ComputeShader* m_JacobiCS;
	ID3D11ComputeShader* m_ProjectionCS;

	//Textures
	ID3D11Texture3D* m_BoundaryConditions;
	ID3D11Texture3D* m_Velocity[2];
	ID3D11Texture3D* m_Density[2];
	ID3D11Texture3D* m_Vorticity;
	ID3D11Texture3D* m_Temperature[2];
	ID3D11Texture3D* m_Divergence;
	ID3D11Texture3D* m_Pressure[2];
	ID3D11Texture3D* m_TempTexture[2];

	//UAVs
	ID3D11UnorderedAccessView* m_BoundaryConditionsUAV;
	ID3D11UnorderedAccessView* m_VelocityUAV[2];
	ID3D11UnorderedAccessView* m_DensityUAV[2];
	ID3D11UnorderedAccessView* m_VorticityUAV;
	ID3D11UnorderedAccessView* m_TemperatureUAV[2];
	ID3D11UnorderedAccessView* m_DivergenceUAV;
	ID3D11UnorderedAccessView* m_PressureUAV[2];
	ID3D11UnorderedAccessView* m_TempUAV[2];

	//SRVs
	ID3D11ShaderResourceView* m_BoundaryConditionsSRV;
	ID3D11ShaderResourceView* m_VelocitySRV[2];
	ID3D11ShaderResourceView* m_DensitySRV[2];
	ID3D11ShaderResourceView* m_VorticitySRV;
	ID3D11ShaderResourceView* m_TemperatureSRV[2];
	ID3D11ShaderResourceView* m_DivergenceSRV;
	ID3D11ShaderResourceView* m_PressureSRV[2];
	ID3D11ShaderResourceView* m_TempSRV[2];

	//buffer
	ID3D11Buffer* m_DensityBuffer;
	ID3D11Buffer* m_ImpulseBuffer;
	ID3D11Buffer* m_AdvectionBuffer;
	ID3D11Buffer* m_BuoyancyBuffer;
	ID3D11Buffer* m_ConfinementBuffer;
};
#endif // FluidShader_h__
