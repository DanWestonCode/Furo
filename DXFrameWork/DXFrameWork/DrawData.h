#ifndef DrawData_h__
#define DrawData_h__

#include <SimpleMath.h>
struct DrawData
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX orthoMatrix;
	DirectX::XMMATRIX projectionMatrix;
};

#endif // DrawData_h__
