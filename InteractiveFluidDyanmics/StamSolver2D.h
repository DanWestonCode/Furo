/*************************************************************/
/*                                                           */
/* TextureFluid is the two dimensional fluid simulation      */
/* in Furo. TextureFluid returns a velocity field for use in */
/* synthesizing 2D textures expressing fluid flows           */
/*                                                           */
/* Created by Daniel Weston 19/12/2015                       */
/*************************************************************/
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

		void SetDensity(int x, int y, float val){ m_prevDensity[x * (m_gridSize + 2) + y] += val; }
		void SetVelX(int x, int y, float val){ m_prevVelX[x * (m_gridSize + 2) + y] += val; }
		void SetVelY(int x, int y, float val){ m_prevVelY[x * (m_gridSize + 2) + y] += val; }

	private:
		FluidSolver2D* m_solver;
	};
}
#endif // TextureFluid_h__