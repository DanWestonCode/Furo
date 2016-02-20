#include "Camera.h"

Camera *Camera::m_Camera = nullptr;

Camera::Camera()
{
	m_pos = XMFLOAT3(0.0f, 1.f, -5.f);
	m_LookAt = XMFLOAT3(0.f, 0.0f, 0.f);
	m_Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
}
Camera::Camera(const Camera& other){}
Camera::~Camera(){}

void Camera::Render()
{
	// Initialize the view matrix
	XMVECTOR eye = XMLoadFloat3(&m_pos);//XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0.0f);
	XMVECTOR at = XMLoadFloat3(&m_LookAt);//XMVectorSet(0.f, 0.0f, 0.f, 0.f);
	XMVECTOR up = XMLoadFloat3(&m_Up);//XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_view = XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up));


	float screenAspect = (float)800 / (float)600;

	// Initialize the projection matrix ~ FoV/Screen Asepct/ Screen Near / Screen Far
	m_projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4, screenAspect, 0.1f, 1000.f));


	// View-projection matrix
	m_ViewProjection = XMMatrixMultiply(m_projection, m_view);
}

void Camera::Update(float dt)
{	
	if (InputManager::Instance()->IsKeyDown(DIK_UP))
		m_pos.z += vel*dt;
	if (InputManager::Instance()->IsKeyDown(DIK_DOWN))
		m_pos.z += -vel*dt;
	if (InputManager::Instance()->IsKeyDown(DIK_LEFT))
		m_pos.x += -vel*dt;
	if (InputManager::Instance()->IsKeyDown(DIK_RIGHT))
		m_pos.x += vel*dt;
	if (InputManager::Instance()->IsKeyDown(DIK_LSHIFT))
		m_pos.y += vel*dt;
	if (InputManager::Instance()->IsKeyDown(DIK_TAB))
		m_pos.y += -vel*dt;
	Object::Update(dt);
}

void Camera::DeleteInstance()
{
	delete m_Camera;
	m_Camera = nullptr;
}

