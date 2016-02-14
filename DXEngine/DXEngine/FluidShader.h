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

private:
	void ComputeBoundaryConditions(ID3D11DeviceContext*);
	void ComputeAdvection(ID3D11DeviceContext*, ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*);
	void ComputeImpulse(ID3D11DeviceContext*, ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*);	

	void ComputeUnbind(ID3D11DeviceContext*, int, int);
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
		m_decay;
public:
	//shaders
	ID3D11ComputeShader* m_BoundaryConditionsCS;
	ID3D11ComputeShader* m_AdvectionCS;
	ID3D11ComputeShader* m_BuoyancyCS;
	ID3D11ComputeShader* m_ImpulseCS;
	//Textures
	ID3D11Texture3D* m_BoundaryConditions;
	ID3D11Texture3D* m_Velocity[2];
	ID3D11Texture3D* m_Density[2];
	ID3D11Texture3D* m_Vorticity[2];
	ID3D11Texture3D* m_Temperature[2];
	//UAVs
	ID3D11UnorderedAccessView* m_BoundaryConditionsUAV;
	ID3D11UnorderedAccessView* m_VelocityUAV[2];
	ID3D11UnorderedAccessView* m_DensityUAV[2];
	ID3D11UnorderedAccessView* m_VorticityUAV[2];
	ID3D11UnorderedAccessView* m_TemperatureUAV[2];
	//SRVs
	ID3D11ShaderResourceView* m_BoundaryConditionsSRV;
	ID3D11ShaderResourceView* m_VelocitySRV[2];
	ID3D11ShaderResourceView* m_DensitySRV[2];
	ID3D11ShaderResourceView* m_VorticitySRV[2];
	ID3D11ShaderResourceView* m_TemperatureSRV[2];

	ID3D11Buffer* m_DensityBuffer;
	ID3D11Buffer* m_AdvectionBuffer;
};
#endif // FluidShader_h__
