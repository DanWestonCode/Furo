#ifndef AABBVolumeRaycastShader_h__
#define AABBVolumeRaycastShader_h__

#include "ShaderBase.h"
#include "Cube.h"

__declspec(align(16)) class AABBVolumeRaycastShader : public ShaderBase
{
protected:
	struct CamBuffer
	{
		XMFLOAT3 CameraPos;
		float padding1;
	};
	struct ObjectBuffer
	{
		XMFLOAT3 ObjectPos;
		float padding2;

		XMFLOAT3 ObjectScale;
		float padding3;
	};
	struct FluidBuffer
	{
		float Absoprtion;
		float padding4;

		int Samples;
		float padding5;
	};
public:
	AABBVolumeRaycastShader();
	AABBVolumeRaycastShader(const AABBVolumeRaycastShader&);
	~AABBVolumeRaycastShader();

	virtual HRESULT Initialize(ID3D11Device*,ID3D11DeviceContext*, HWND);
	virtual void Shutdown();

	void InitilializeBuffers(ID3D11Device*, ID3D11DeviceContext*, Cube*);

	//Buffers for AABB RayCast
	ID3D11Buffer* m_CamBuffer;
	ID3D11Buffer* m_ObjectBuffer;
	ID3D11Buffer* m_FluidBuffer;
};

#endif // VolumeRaycastShader_h__