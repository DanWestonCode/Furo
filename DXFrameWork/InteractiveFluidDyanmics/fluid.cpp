/*************************************************************/
/*                                                           */
/* Fluid is the base class for all fluid simulations created */
/* using Furo. The fluid base class contains base member var */
/* -iables which are shared between both 3D and 2D based sim */
/*                                                           */
/* Created by Daniel Weston 19/12/2015                       */
/*************************************************************/

#include "fluid.h"
#include<cstring>

Fluid::Fluid()
{
	//set pointers to null
	m_velocityX = 0;
	m_velocityY = 0;
	m_prevVelX = 0;
	m_prevVelY = 0;
	m_density = 0;
	m_prevDensity = 0;

	m_fluidSolver = 0;
}

Fluid::~Fluid()
{
}

//initialize fluid - set up all vars w/default 
void Fluid::Initialize(int _size, float _dt)
{
	//set up fluid props
	m_gridSize = _size;
	m_dt = _dt;
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

	std::memset(m_velocityX, 0, sizeof(float)*size);
	std::memset(m_velocityY, 0, sizeof(float)*size);
	std::memset(m_prevVelX, 0, sizeof(float)*size);
	std::memset(m_prevVelY, 0, sizeof(float)*size);
	std::memset(m_density, 0, sizeof(float)*size);
	std::memset(m_prevDensity, 0, sizeof(float)*size);

	m_fluidSolver = new FluidSolver;

	
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
	if (m_fluidSolver)
	{
		delete m_fluidSolver;
		m_fluidSolver = 0;
	}
}

int Fluid::GetIndex(int x, int y)
{
	return (x * (m_gridSize + 2)) + y;
}

void Fluid::Clear()
{

}
