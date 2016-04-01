/*************************************************************/
/*                                                           */
/* Fluid is the base class for all fluid simulations created */
/* using Furo. The fluid base class contains base member var */
/* -iables which are shared between both 3D and 2D based sim */
/*                                                           */
/* Created by Daniel Weston 19/12/15                         */
/*************************************************************/

#include "fluid.h"
#include <cstring>

Fluid::Fluid()
{
	//set pointers to null
	m_velocityX = nullptr;
	m_velocityY = nullptr;
	m_velocityZ = nullptr;

	m_prevVelX = nullptr;
	m_prevVelY = nullptr;
	m_prevVelZ = nullptr;

	m_density = nullptr;
	m_prevDensity = nullptr;

	m_fluidSolver2D = nullptr;
}

Fluid::~Fluid()
{
}

//Clean up all initialized pointers - call when game ends
void Fluid::Shutdown()
{
	if (m_velocityX)
	{
		delete m_velocityX;
		m_velocityX = nullptr;
	}
	if (m_velocityY)
	{
		delete m_velocityY;
		m_velocityY = nullptr;
	}
	if (m_prevVelX)
	{
		delete m_prevVelX;
		m_prevVelX = nullptr;
	}
	if (m_prevVelY)
	{
		delete m_prevVelY;
		m_prevVelY = nullptr;
	}
	if (m_density)
	{
		delete m_density;
		m_density = nullptr;
	}
	if (m_prevDensity)
	{
		delete m_prevDensity;
		m_prevDensity = nullptr;
	}
	if (m_prevDensity)
	{
		delete m_prevDensity;
		m_prevDensity = nullptr;
	}
	if (m_fluidSolver2D)
	{
		delete m_fluidSolver2D;
		m_fluidSolver2D = nullptr;
	}
}