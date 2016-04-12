/// <summary>
/// StamSolver.cpp
///
/// About:
/// StamSolver.cpp is the base class for both the 2D and 3D CPU 
/// methods within Furo. The class is utilized as a base class,
/// containing variables common between the two simulations
/// </summary>
#include "StamSolver.h"
using namespace Furo;

void StamSolver::Initialize()
{
	//set pointers to null
	m_velocityX = nullptr;
	m_velocityY = nullptr;

	m_prevVelX = nullptr;
	m_prevVelY = nullptr;

	m_density = nullptr;
	m_prevDensity = nullptr;
}

void StamSolver::Shutdown()
{
	delete m_velocityX;
	m_velocityX = nullptr;

	delete m_velocityY;
	m_velocityY = nullptr;

	delete m_prevVelX;
	m_prevVelX = nullptr;

	delete m_prevVelY;
	m_prevVelY = nullptr;

	delete m_density;
	m_density = nullptr;

	delete m_prevDensity;
	m_prevDensity = nullptr;

	delete m_prevDensity;
	m_prevDensity = nullptr;
}