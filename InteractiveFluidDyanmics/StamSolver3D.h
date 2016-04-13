/// <summary>
/// StamSolver3D.h
///
/// About:
/// StamSolver3D.h wraps the functionality of FluidSovler3D.
/// The class is intended as FluidSolver3D's point of access
/// for external use
/// </summary>
#ifndef FluidThreeDimensional_h__
#define FluidThreeDimensional_h__
#include "StamSolver.h"
#include "FluidSolver3D.h"
namespace Furo
{
	class StamSolver3D : public StamSolver
	{
	public:
		void Run(float);
		void Clear();
		void Initialize(int);
		void ShutDown();
		void Empty();

		void SetDensity(int x, int y, int z, float val){ m_density[((x)+(y)* m_gridSize + (z)* m_gridSize * m_gridSize)] += val; }
		void SetVelX(int x, int y, int z, float val){ m_velocityX[((x)+(y)* m_gridSize + (z)* m_gridSize * m_gridSize)] += val; }
		void SetVelY(int x, int y, int z, float val){ m_velocityY[((x)+(y)* m_gridSize + (z)* m_gridSize * m_gridSize)] += val; }

		void SetVelZ(int x, int y, int z, float val){ m_velocityZ[((x)+(y)* m_gridSize + (z)* m_gridSize * m_gridSize)] += val; }
	protected:
		float* m_prevVelZ;
		float* m_velocityZ;

	private:
		FluidSolver3D* m_solver;
	};
}
#endif // FluidThreeDimensional_h__
