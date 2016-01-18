#include "FluidTwoDimensional.h"
#include <cstring>

void FluidTwoDimensional::Initialize(int _size)
{
	//set up fluid props
	m_gridSize = _size;
	m_diffusion = 0.0f;
	m_visc = 0.0f;

	int size = (_size + 2) * (_size + 2);

	//set up fluid structures
	m_velocityX = new float[size];
	m_velocityY = new float[size];
	m_prevVelX = new float[size];
	m_prevVelY = new float[size];
	m_density = new float[size];
	m_prevDensity = new float[size];

	//set all arrays to 0
	std::memset(m_velocityX, 0, sizeof(float)*size);
	std::memset(m_velocityY, 0, sizeof(float)*size);
	std::memset(m_prevVelX, 0, sizeof(float)*size);
	std::memset(m_prevVelY, 0, sizeof(float)*size);
	std::memset(m_density, 0, sizeof(float)*size);
	std::memset(m_prevDensity, 0, sizeof(float)*size);

	m_fluidSolver2D = new FluidSolver2D;
}

void FluidTwoDimensional::Update(float _dt)
{
	m_fluidSolver2D->VelocityStep(m_gridSize, m_velocityX, m_prevVelX, m_velocityY, m_prevVelY, m_visc, _dt);
	m_fluidSolver2D->DensityStep(m_gridSize, m_density, m_prevDensity, m_velocityX, m_velocityY, m_diffusion, _dt);
}

void FluidTwoDimensional::Clear()
{
	int size = (m_gridSize + 2) * (m_gridSize + 2);
	//change to memset
	std::memset(m_prevVelX, 0, sizeof(float)*size);
	std::memset(m_prevVelY, 0, sizeof(float)*size);
	std::memset(m_prevDensity, 0, sizeof(float)*size);
}







