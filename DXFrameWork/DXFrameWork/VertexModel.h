#ifndef _MODEL_H_
#define _MODEL_H_

#include <d3d11.h>

#include "TextureLoader.h"
#include "textureshader.h"
#include "ColorShader.h"
#include "Object.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

//Fix for 'warning C4316: object allocated on the heap may not be aligned 16'
__declspec(align(16)) class VertexModel : public Object
{
protected:
	//vertex buffer layout for texture shader
	struct TextureVL
	{
		Vector3 position;
		Color color;
		Vector2 texture;
	};
	//vertex buffer layout for color shader
	struct ColorVL
	{
		Vector3 position;
		Color color;
	};

public:
	VertexModel();
	VertexModel(const VertexModel&);
	~VertexModel();

	//inherited functions
	virtual HRESULT Initialize(ID3D11Device*, WCHAR*){ return S_OK; };
	virtual void Update(float dt){ Object::Update(dt); };
	virtual void Shutdown();

	virtual void Render(ID3D11DeviceContext*);

	//getters/setters
	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

private:
	void RenderBuffers(ID3D11DeviceContext*);
		
protected:
	//Build index and vertex buffers
	HRESULT BuildDynamicVB(ID3D11Device*, int, void*);
	HRESULT BuildImmuatbleVB(ID3D11Device*, int, void*);
	HRESULT BuildStaticVB(ID3D11Device*, int, void*);
	HRESULT BuildIndexBuffer(ID3D11Device*, void*);
	HRESULT BuildImmuatbleIB(ID3D11Device*, void*);

	HRESULT LoadTexture(ID3D11Device*, WCHAR*);

	TextureVL* m_VerticesTextureVL;
	ColorVL* m_VerticesColorVL;

	TextureShader* m_TextureShader;
	ColorShader* m_ColorShader;

	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;
	ID3D11ShaderResourceView* m_Texture;
	int m_VertexCount, m_IndexCount;

	void ReleaseTexture();	
};

#endif