/// <summary>
/// StamSolver3D.cpp
///
/// About:
/// StamSolver3D.cpp wraps the functionality of FluidSovler3D.
/// The class is intended as FluidSolver3D's point of access
/// for external use
/// </summary>
#include "StamSolver3D.h"
#include <cstring>
using namespace Furo;

void StamSolver3D::Initialize(int _size)
{
	StamSolver::Initialize();

	//set up fluid props
	m_gridSize = _size;
	m_diffusion = 0.0f;
	m_visc = 0.0f;

	m_velocityZ = nullptr;
	m_prevVelZ = nullptr;

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

	m_solver = new FluidSolver3D;
}

void StamSolver3D::Run(float dt)
{
	m_solver->SimStep(m_prevVelX, m_prevVelY, m_prevVelZ, m_velocityX, m_velocityY, m_velocityZ, m_visc, dt, m_density, m_diffusion, m_prevDensity, m_gridSize);
}

void StamSolver3D::Clear()
{
	int size = (m_gridSize)* (m_gridSize)* (m_gridSize);
	std::memset(m_prevVelX, 0, sizeof(float)*size);
	std::memset(m_prevVelY, 0, sizeof(float)*size);
	std::memset(m_prevVelZ, 0, sizeof(float)*size);
	std::memset(m_prevDensity, 0, sizeof(float)*size);
}

void StamSolver3D::Empty()
{
	int size = (m_gridSize)* (m_gridSize)* (m_gridSize);
	//change to memset
	std::memset(m_prevVelX, 0, sizeof(float)*size);
	std::memset(m_prevVelY, 0, sizeof(float)*size);
	std::memset(m_prevDensity, 0, sizeof(float)*size);
	std::memset(m_prevVelZ, 0, sizeof(float)*size);

	std::memset(m_density, 0, sizeof(float)*size);
	std::memset(m_velocityY, 0, sizeof(float)*size);
	std::memset(m_velocityX, 0, sizeof(float)*size);
	std::memset(m_velocityZ, 0, sizeof(float)*size);
}

void StamSolver3D::ShutDown()
{
	StamSolver::Shutdown();
	delete m_prevVelY;
	m_prevVelY = nullptr;
}