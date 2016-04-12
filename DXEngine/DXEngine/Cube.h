/// <summary>
/// cube.h
///
/// About:
/// Cube.h derives from VertexObject. The class creates a cube VO object.
/// The implementation of the vertices and indicies for the cube was 
/// guided by a similar implementation within Dr.Christian B Mendl project - 
/// 'Volume Ray-Casting Renderer project'
///
/// References:
/// Dr.Christian B Mendl: http://christian.mendl.net/pages/software.html - See 'volume_raycast.cpp' in 'Volume Ray-Casting Renderer project'
///</summary>
#ifndef Cube_h__
#define Cube_h__

#include "VertexObject.h"

__declspec(align(16)) class Cube : public VertexObject
{
public:
	Cube();
	~Cube();

	virtual HRESULT Initialise(ID3D11Device*);
	virtual void Render(ID3D11DeviceContext*);
	virtual void Shutdown();
	virtual void Update(float);

public:
	ID3D11Buffer* m_IndexBuffer;
	ID3D11Buffer* m_VertexBuffer;
};
#endif // Cube_h__
