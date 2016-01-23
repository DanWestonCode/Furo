#include "FluidThreeDimensional.h"
#include <cstring>
void FluidThreeDimensional::Initialize(int _size)
{
	//set up fluid props
	m_gridSize = _size;
	m_diffusion = 0.0f;
	m_visc = 0.0f;

	int size = (_size) * (_size) * (_size);

	//set up fluid structures
	m_velocityX = new float[size];
	m_velocityY = new float[size];
	m_velocityZ = new float[size];

	m_prevVelX = new float[size];
	m_prevVelY = new float[size];
	m_prevVelZ = new float[size];

	m_density = new float[size];
	m_prevDensity = new float[size];

	//set all arrays to 0
	std::memset(m_velocityX, 0, sizeof(float)*size);
	std::memset(m_velocityY, 0, sizeof(float)*size);
	std::memset(m_velocityZ, 0, sizeof(float)*size);

	std::memset(m_prevVelX, 0, sizeof(float)*size);
	std::memset(m_prevVelY, 0, sizeof(float)*size);
	std::memset(m_prevVelZ, 0, sizeof(float)*size);

	std::memset(m_density, 0, sizeof(float)*size);
	std::memset(m_prevDensity, 0, sizeof(float)*size);

	m_fluidSolver3D = new FluidSolver3D;
}
void FluidThreeDimensional::Update(float dt)
{
	m_fluidSolver3D->SimStep(m_prevVelX, m_prevVelY, m_prevVelZ, m_velocityX, m_velocityY, m_velocityZ, m_visc, dt, m_density, m_diffusion, m_prevDensity, m_gridSize);
}

void FluidThreeDimensional::Clear()
{
	int size = (m_gridSize + 2) * (m_gridSize + 2);
	//change to memset
	std::memset(m_prevVelX, 0, sizeof(float)*size);
	std::memset(m_prevVelY, 0, sizeof(float)*size);
	std::memset(m_prevDensity, 0, sizeof(float)*size);
}


