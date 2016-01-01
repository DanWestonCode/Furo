#include "Object.h"

Object::Object()
{
	m_pos = Vector3::Zero;
	m_rot = Vector3::Zero;
	m_scale = Vector3::One;
	m_worldMatrix = Matrix::Identity;
	vel = 10.0f;
};
Object::~Object(){};

void Object::Update(float dt)
{
	Matrix  scaleMat = Matrix::CreateScale(m_scale);
	m_rotationMatrix = Matrix::CreateFromYawPitchRoll(m_rot.x, m_rot.y, m_rot.z);
	Matrix transMat = Matrix::CreateTranslation(m_pos);

	m_worldMatrix = scaleMat*m_rotationMatrix*transMat;
}


