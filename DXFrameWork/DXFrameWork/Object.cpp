#include "Object.h"

Object::Object()
{
	m_pos = Vector3::Zero;
	m_rot = Vector3::Zero;
	m_scale = Vector3::One;
	m_worldMatrix = Matrix::Identity;
	vel = 20.0f;
};
Object::~Object(){};


//Fix for 'warning C4316: object allocated on the heap may not be aligned 16'
//This kept giving me access violation errors using XMMatrix calculations
//functions discovered from:-
//http://stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16
void* Object::operator new(size_t memorySize)
{
	return _aligned_malloc(memorySize, 16);

}

void Object::operator delete(void* memoryBlockPtr)
{
	_aligned_free(memoryBlockPtr);
	return;
}


void Object::Update(float dt)
{
	Matrix  scaleMat = Matrix::CreateScale(m_scale);
	m_rotationMatrix = Matrix::CreateFromYawPitchRoll(m_rot.x, m_rot.y, m_rot.z);
	Matrix transMat = Matrix::CreateTranslation(m_pos);

	Matrix m_newWorld = scaleMat*m_rotationMatrix*transMat;
	m_worldMatrix = m_newWorld;
}


