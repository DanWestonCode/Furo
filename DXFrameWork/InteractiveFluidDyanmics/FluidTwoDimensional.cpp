#include "FluidTwoDimensional.h"

void FluidTwoDimensional::Update()
{
	m_fluidSolver->VelocityStep(m_gridSize, m_velocityX, m_prevVelX, m_velocityY,  m_prevVelY, m_visc, m_dt);
	m_fluidSolver->DensityStep(m_gridSize, m_density, m_prevDensity, m_velocityX, m_velocityY, m_diffusion, m_dt);
}







