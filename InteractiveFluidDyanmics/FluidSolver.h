#ifndef FluidSolver_h__
#define FluidSolver_h__

class FluidSolver
{
public:
	FluidSolver();
	~FluidSolver();

	void VelocityStep(int, float*, float*, float*, float*, float, float);
	void DensityStep(int, float*, float*, float*, float*, float, float);
	void Diffuse(int, int , float*, float*, float, float);
	void Advect(int, int, float*, float*, float*, float*, float);
	void Project(int, float*, float*, float*, float*);
	void SetBnds(int, int, float*);
	void AddSource(int, float*, float*, float);

	void Swap(float*_x, float* _y);
};


#endif // FluidSolver_h__
