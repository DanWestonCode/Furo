#ifndef FluidShader_h__
#define FluidShader_h__

#include "ShaderBase.h"
#include "RenderTexture.h"
#include <utility>

__declspec(align(16)) class FluidShader : public ShaderBase
{
protected:
	void CompileShaders(ID3D11Device*);
	void CreateTextures(ID3D11Device*);
	void CreateSampler(ID3D11Device*);

protected:
	struct FluidBuffer{
		float dt;
		float fDensityBuoyancy;
		float fDensityWeight;
		float fVorticityStrength;
		XMFLOAT3 vBuoyancyDirection;
		float padding;
	};
	struct AdvectionBuffer {
		float dissapation;
		float dtMod;
		float decay;
		float padding;
	};
	struct ImpulseBuffer {
		XMFLOAT3 vPoint;
		float fRadius;
		XMFLOAT3 vAmount;
		float fExtinguishment;
	};


public:
	FluidShader();
	FluidShader(const FluidShader&);
	~FluidShader();

	virtual HRESULT Initialize(ID3D11Device*, ID3D11DeviceContext*);
	virtual void Shutdown();
	void Update(ID3D11DeviceContext*,float);

private:
	void ComputeBoundaryConditions(ID3D11DeviceContext*);
	void ComputeAdvection(ID3D11DeviceContext*, ID3D11UnorderedAccessView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float);
	void ComputeUnbind(ID3D11DeviceContext*, int, int);

public:
	//shaders
	ID3D11ComputeShader* m_BoundaryConditionsCS;
	ID3D11ComputeShader* m_AdvectionCS;

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
	
	//sampler
	ID3D11SamplerState* m_Sampler;

	//buffers
	FluidBuffer* m_fluidBufferObj;
	AdvectionBuffer* m_AdvectionBufferObj;
	ImpulseBuffer* m_ImpulseBufferObj;

	ID3D11Buffer* m_FluidBuffer;
	ID3D11Buffer* m_AdvectionBuffer;
	ID3D11Buffer* m_ImpulseBuffer;

	int m_volSize;
};
#endif // FluidShader_h__
