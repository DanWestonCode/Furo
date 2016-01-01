#include "FluidThreeDimensional.h"
#include <cstring>
void FluidTwoDimensional::Update(float dt)
{
	m_fluidSolver->VelocityStep(m_gridSize, m_velocityX, m_prevVelX, m_velocityY, m_prevVelY, m_visc, dt);
	m_fluidSolver->DensityStep(m_gridSize, m_density, m_prevDensity, m_velocityX, m_velocityY, m_diffusion, dt);
}

void FluidTwoDimensional::Clear()
{
	int size = (m_gridSize + 2) * (m_gridSize + 2);
	//change to memset
	std::memset(m_prevVelX, 0, sizeof(float)*size);
	std::memset(m_prevVelY, 0, sizeof(float)*size);
	std::memset(m_prevDensity, 0, sizeof(float)*size);
}


