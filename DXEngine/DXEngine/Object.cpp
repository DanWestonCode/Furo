/// <summary>
/// Object.cpp
///
/// About:
/// Object is the base class for all Game objects created 
/// within the engine
/// </summary>
#include "Object.h"

Object::Object()
{
	m_pos = XMFLOAT3(0, 0, 0);
	m_rot = XMFLOAT3(0, 0, 0);
	m_scale = XMFLOAT3(1, 1, 1);
	m_worldMatrix = XMMatrixIdentity();
	vel = 5.0f;
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
	XMMATRIX scaleMat = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	m_rotationMatrix = XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z);
	XMMATRIX transMat = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	XMMATRIX m_newWorld = scaleMat*m_rotationMatrix*transMat;

	m_worldMatrix = m_newWorld;
}