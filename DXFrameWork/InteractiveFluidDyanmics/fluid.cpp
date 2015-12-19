#include "fluid.h"

Fluid::Fluid()
{
	//set pointers to null
	m_velocityX = 0;
	m_velocityY = 0;
	m_prevVelX = 0;
	m_prevVelY = 0;
	m_density = 0;
	m_prevDensity = 0;
}

Fluid::~Fluid()
{
}

//initialize fluid - set up all vars w/default 
void Fluid::Initialize(int _size, float _dt)
{
	m_gridSize = _size;
	m_dt = _dt;
	m_diffusion = 0.0f;
	m_visc = 0.0f;
}

//initialize fluid override - set up all vars w/custom 
void Fluid::Initialize(int _size, float _dt, float _diff, float _visc)
{
	m_gridSize = _size;
	m_dt = _dt;
	m_diffusion = _diff;
	m_visc = _visc;
}

//Clean up all initialized pointers - call when game ends
void Fluid::Shutdown()
{
	if (m_velocityX)
	{
		delete m_velocityX;
		m_velocityX = 0;
	}
	if (m_velocityY)
	{
		delete m_velocityY;
		m_velocityY = 0;
	}
	if (m_prevVelX)
	{
		delete m_prevVelX;
		m_prevVelX = 0;
	}
	if (m_prevVelY)
	{
		delete m_prevVelY;
		m_prevVelY = 0;
	}
	if (m_density)
	{
		delete m_density;
		m_density = 0;
	}
	if (m_prevDensity)
	{
		delete m_prevDensity;
		m_prevDensity = 0;
	}
	if (m_prevDensity)
	{
		delete m_prevDensity;
		m_prevDensity = 0;
	}
}
