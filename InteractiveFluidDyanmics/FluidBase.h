/*************************************************************/
/*                                                           */
/* Fluid is the base class for all fluid simulations created */
/* using Furo. The fluid base class contains base member var */
/* -iables which are shared between both 3D and 2D based sim */
/*                                                           */
/* Created by Daniel Weston 19/12/2015                       */
/*************************************************************/
#ifndef _FLUIDBASE_H_
#define _FLUIDBASE_H_
namespace Furo
{
	class FluidBase
	{
	public:
		FluidBase(){};
		~FluidBase(){};
	public:
		virtual void Initialize(){};
		virtual void Run(){};
		virtual void Shutdown(){};
	};
}
#endif