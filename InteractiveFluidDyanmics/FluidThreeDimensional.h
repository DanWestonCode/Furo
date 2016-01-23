#ifndef FluidThreeDimensional_h__
#define FluidThreeDimensional_h__
#include "fluid.h"
class FluidThreeDimensional : public Fluid
{
public:
	void Update(float);
	virtual void Initialize(int);
	virtual void Clear();

	virtual void SetDensity(int x, int y, int z, float val){ m_density[((x)+(y)* m_gridSize + (z)* m_gridSize * m_gridSize)] += val; }
	virtual void SetVelX(int x, int y, int z, float val){ m_velocityX[((x)+(y)* m_gridSize + (z)* m_gridSize * m_gridSize)] += val; }
	virtual void SetVelY(int x, int y, int z, float val){ m_velocityY[((x)+(y)* m_gridSize + (z)* m_gridSize * m_gridSize)] += val; }

	virtual void SetVelZ(int x, int y, int z, float val){ m_velocityZ[((x)+(y)* m_gridSize + (z)* m_gridSize * m_gridSize)] += val; }

};
#endif // FluidThreeDimensional_h__
