#ifndef DebugWindow_h__
#define DebugWindow_h__

#include <d3d11.h>
#include <SimpleMath.h>

#include "VertexModel.h"

class DebugWindow : public VertexModel
{
public:
	DebugWindow();
	DebugWindow(const DebugWindow&);
	~DebugWindow();

	virtual HRESULT Initialize(ID3D11Device*, int, int, int, int, HWND);
	virtual void Shutdown();
	virtual void Render(ID3D11DeviceContext*, int, int, XMMATRIX*, XMMATRIX*, XMMATRIX*, ID3D11ShaderResourceView*);

	void InitializeBuffers(ID3D11Device*, HWND);
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);

private:
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

#endif // DebugWindow_h__
