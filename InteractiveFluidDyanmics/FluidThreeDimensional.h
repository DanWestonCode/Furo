#ifndef FluidThreeDimensional_h__
#define FluidThreeDimensional_h__
#include "fluid.h"
class FluidTwoDimensional : public Fluid
{
public:
	void Update(float);
	virtual void Clear();
};
#endif // FluidThreeDimensional_h__
