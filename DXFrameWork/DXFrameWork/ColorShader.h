#ifndef _COLORSHADER_H_
#define _COLORSHADER_H_


#include <d3d11.h>
#include <d3dcompiler.h>
#include <SimpleMath.h>
#include <fstream>
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

class ColorShader
{
private:

	//definition for buffer to be used with shader
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

public:
	ColorShader();
	ColorShader(const ColorShader&);
	~ColorShader();
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, Matrix, Matrix, Matrix);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, Matrix, Matrix, Matrix);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11RasterizerState* backfacecull;
	ID3D11RasterizerState* frontfacecull;

};

#endif