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
#include "fluid.h"

class FluidTwoDimensional : public Fluid
{
public:
	void Update(float);
	virtual void Initialize(int);
	virtual void Clear();

	virtual void SetDensity(int x, int y, float val){ m_prevDensity[x * (m_gridSize + 2) + y] += val; }
	virtual void SetVelX(int x, int y, float val){ m_prevVelX[x * (m_gridSize + 2) + y] += val; }
	virtual void SetVelY(int x, int y, float val){ m_prevVelY[x * (m_gridSize + 2) + y] += val; }
};
#endif // TextureFluid_h__
