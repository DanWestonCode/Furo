/// <summary>
/// StamSolver2D.h
///
/// About:
/// StamSolver2D.h wraps the functionality of FluidSovler2D.
/// The class is intended as FluidSolver2D's point of access
/// for external use
/// </summary>
#ifndef FluidTwoDimensional_h__
#define FluidTwoDimensional_h__
#include "StamSolver.h"
#include "FluidSolver2D.h"

namespace Furo
{
	class StamSolver2D : public StamSolver
	{
	public:
		void Run(float);
		void Clear();
		void Initialize(int);
		void Empty();

		void SetDensity(int x, int y, float val){ m_prevDensity[x * (m_gridSize + 2) + y] += val; }
		void SetVelX(int x, int y, float val){ m_prevVelX[x * (m_gridSize + 2) + y] += val; }
		void SetVelY(int x, int y, float val){ m_prevVelY[x * (m_gridSize + 2) + y] += val; }

	private:
		FluidSolver2D* m_solver;
	};
}
#endif // TextureFluid_h__