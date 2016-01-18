#ifndef FluidThreeDimensional_h__
#define FluidThreeDimensional_h__
#include "fluid.h"
class FluidThreeDimensional : public Fluid
{
public:
	void Update(float);
	virtual void Initialize(int);
	virtual void Clear();
};
#endif // FluidThreeDimensional_h__
