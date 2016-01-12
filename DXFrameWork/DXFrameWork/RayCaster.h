#ifndef RayCaster_h__
#define RayCaster_h__

#include <d3d11.h>
#include <d3dcompiler.h>
#include <SimpleMath.h>
#include <fstream>
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

class RayCaster
{
protected:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix proj;
	};

	struct VertexLayout
	{
		Vector3 position;
		Color color;
	};

public:
	RayCaster();
	RayCaster(const RayCaster&);
	~RayCaster();
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, Matrix, Matrix, Matrix);

	void SetBackBuffer(ID3D11RenderTargetView* srv){ backbuffer = srv; }

private:
	void InitializeStandardShader(ID3D11Device*, HWND);
	void InitializeRayCastShader(ID3D11Device*, HWND);
	void CreateConstantBuffer(ID3D11Device*);
	void InitRenderTextures(ID3D11Device*, int, int);
	void InitRasterStates(ID3D11Device*);
	void InitVolumeTexture(ID3D11Device*);
	void InitSampler(ID3D11Device*);
	void InitRenderCube(ID3D11Device*);
	void BuildImmuatbleVB(ID3D11Device*, int, void*);
	void BuildImmuatbleIB(ID3D11Device*, void*);
	void RenderShader(ID3D11DeviceContext*, int);

	void Update(float dt);

private:
	ID3D11VertexShader* m_RayCasterVS;
	ID3D11PixelShader* m_RayCasterPS;
	ID3D11VertexShader* m_StandardVS;
	ID3D11PixelShader* m_StandardPS;
	ID3D11InputLayout* m_RayCasterLayout;
	ID3D11InputLayout* m_StandardLayout;
	ID3D11Buffer* m_matrixBuffer;

	ID3D11Texture2D* m_Texture2D[2];
	ID3D11ShaderResourceView* m_ShaderResource[2];
	ID3D11RenderTargetView*	m_RenderTargetView[2];

	ID3D11RasterizerState* backfacecull;
	ID3D11RasterizerState* frontfacecull;

	ID3D11Texture3D* m_Texture3D = NULL;
	ID3D11ShaderResourceView* m_VolumeShaderRV = NULL;

	ID3D11SamplerState* m_SamplerState;

	VertexLayout* m_vertices;
	int m_VertexCount, m_IndexCount;
	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;

	ID3D11RenderTargetView* backbuffer;
	
};
#endif // RayCaster_h__
