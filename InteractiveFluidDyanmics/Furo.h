#ifndef Furo_h__
#define Furo_h__

#include "FluidTwoDimensional.h"
#include "FluidThreeDimensional.h"
class Furo
{
public:
	enum FluidField
	{
		TwoDimensional,
		ThreeDimensional
	};
public:
	Furo();
	~Furo();

	void Initialize(FluidField, int, float);
	void Shutdown();

	void Run(float);

	Fluid* GetFluid();

//private:
	FluidTwoDimensional* m_textureFluid;
	FluidThreeDimensional* m_volumeFluid;
	FluidField m_simulationType;
};

#endif // Furo_h__
