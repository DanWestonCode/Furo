#ifndef Camera_h__
#define Camera_h__

#include "Object.h"

__declspec(align(16)) class Camera : public Object
{
public:
	static Camera *Instance()
	{
		if (!m_Camera)
		{
			m_Camera = new Camera;
		}
		return m_Camera;
	}


	Camera();
	Camera(const Camera&);
	~Camera();

	virtual void Render();
	virtual void Update(float);

public:
	XMFLOAT3 m_Up, m_LookAt;
	XMMATRIX GetViewProj(){ return m_ViewProjection; }
	void DeleteInstance();
protected:
	XMMATRIX m_ViewProjection;

	static Camera *m_Camera;
};
#endif // Camera_h__
