#ifndef FluidSolver3D_h__
#define FluidSolver3D_h__

class FluidSolver3D
{
public:
	FluidSolver3D();
	~FluidSolver3D();

	void SimStep(float* Vx0, float * Vy0, float* Vz0, float* Vx, float* Vy, float* Vz, float visc, float dt, float* density, float diff, float* s, int N);

	void set_bnd(int b, float *x, int N);

	void lin_solve(int b, float *x, float *x0, float a, float c, int iter, int N);

	void diffuse(int b, float *x, float *x0, float diff, float dt, int iter, int N);

	void advect(int b, float *d, float *d0, float *velocX, float *velocY, float *velocZ, float dt, int N);

	void project(float *velocX, float *velocY, float *velocZ, float *p, float *div, int iter, int N);

};
#endif // FluidSolver3D_h__
