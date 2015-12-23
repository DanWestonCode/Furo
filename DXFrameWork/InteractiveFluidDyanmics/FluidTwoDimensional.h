/*************************************************************/
/*                                                           */
/* TextureFluid is the two dimensional fluid simulation      */
/* in Furo. TextureFluid returns a velocity field for use in */
/* synthesizing 2D textures expressing fluid flows           */
/*                                                           */
/* Created by Daniel Weston 19/12/2015                       */
/*************************************************************/
#ifndef TextureFluid_h__
#define TextureFluid_h__
#include "fluid.h"

class FluidTwoDimensional : public Fluid
{
public:
	void Update(float);
	virtual void Clear();


	bool _switch = false;
	int counter = 0;
};



#endif // TextureFluid_h__
