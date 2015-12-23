#include "FluidTwoDimensional.h"
#include <cstring>
void FluidTwoDimensional::Update(float dt)
{	
	/*if (!_switch)
	{*/
	/*m_prevDensity[1 * (m_gridSize + 2) + 1] = 1.0f;
	m_prevVelX[1 * (m_gridSize + 2) + 1] = 600.0f;
	m_prevVelY[1 * (m_gridSize + 2) + 1] = 600.0f;*/
		//_switch = !_switch;
	//}
	
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







