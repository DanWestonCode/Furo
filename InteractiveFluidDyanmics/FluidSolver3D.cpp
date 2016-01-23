#include "FluidSolver3D.h"

#include <algorithm>    
#define GetXYZ(gridSize,x,y,z) ((x) + (y)* gridSize + (z) * gridSize * gridSize)

FluidSolver3D::FluidSolver3D()
{

}

FluidSolver3D::~FluidSolver3D()
{

}


void FluidSolver3D::SimStep(float* Vx0, float * Vy0, float* Vz0, float* Vx, float* Vy, float* Vz, float visc, float dt, float* density, float diff, float* s, int N)
{

	diffuse(1, Vx0, Vx, visc, dt, 4, N);
	diffuse(2, Vy0, Vy, visc, dt, 4, N);
	diffuse(3, Vz0, Vz, visc, dt, 4, N);

	project(Vx0, Vy0, Vz0, Vx, Vy, 4, N);

	advect(1, Vx, Vx0, Vx0, Vy0, Vz0, dt, N);
	advect(2, Vy, Vy0, Vx0, Vy0, Vz0, dt, N);
	advect(3, Vz, Vz0, Vx0, Vy0, Vz0, dt, N);

	project(Vx, Vy, Vz, Vx0, Vy0, 4, N);

	diffuse(0, s, density, diff, dt, 4, N);
	advect(0, density, s, Vx, Vy, Vz, dt, N);
}


 void FluidSolver3D::set_bnd(int b, float *x, int N)
{
	for (int j = 1; j < N - 1; j++) {
		for (int i = 1; i < N - 1; i++) {
			x[GetXYZ(N,i, j, 0)] = b == 3 ? -x[GetXYZ(N,i, j, 1)] : x[GetXYZ(N,i, j, 1)];
			x[GetXYZ(N,i, j, N - 1)] = b == 3 ? -x[GetXYZ(N,i, j, N - 2)] : x[GetXYZ(N,i, j, N - 2)];
		}
	}
	for (int k = 1; k < N - 1; k++) {
		for (int i = 1; i < N - 1; i++) {
			x[GetXYZ(N,i, 0, k)] = b == 2 ? -x[GetXYZ(N,i, 1, k)] : x[GetXYZ(N,i, 1, k)];
			x[GetXYZ(N,i, N - 1, k)] = b == 2 ? -x[GetXYZ(N,i, N - 2, k)] : x[GetXYZ(N,i, N - 2, k)];
		}
	}
	for (int k = 1; k < N - 1; k++) {
		for (int j = 1; j < N - 1; j++) {
			x[GetXYZ(N,0, j, k)] = b == 1 ? -x[GetXYZ(N,1, j, k)] : x[GetXYZ(N,1, j, k)];
			x[GetXYZ(N,N - 1, j, k)] = b == 1 ? -x[GetXYZ(N,N - 2, j, k)] : x[GetXYZ(N,N - 2, j, k)];
		}
	}

	x[GetXYZ(N,0, 0, 0)] = 0.33f * (x[GetXYZ(N,1, 0, 0)]
		+ x[GetXYZ(N,0, 1, 0)]
		+ x[GetXYZ(N,0, 0, 1)]);
	x[GetXYZ(N,0, N - 1, 0)] = 0.33f * (x[GetXYZ(N,1, N - 1, 0)]
		+ x[GetXYZ(N,0, N - 2, 0)]
		+ x[GetXYZ(N,0, N - 1, 1)]);
	x[GetXYZ(N,0, 0, N - 1)] = 0.33f * (x[GetXYZ(N,1, 0, N - 1)]
		+ x[GetXYZ(N,0, 1, N - 1)]
		+ x[GetXYZ(N,0, 0, N)]);
	x[GetXYZ(N,0, N - 1, N - 1)] = 0.33f * (x[GetXYZ(N,1, N - 1, N - 1)]
		+ x[GetXYZ(N,0, N - 2, N - 1)]
		+ x[GetXYZ(N,0, N - 1, N - 2)]);
	x[GetXYZ(N,N - 1, 0, 0)] = 0.33f * (x[GetXYZ(N,N - 2, 0, 0)]
		+ x[GetXYZ(N,N - 1, 1, 0)]
		+ x[GetXYZ(N,N - 1, 0, 1)]);
	x[GetXYZ(N,N - 1, N - 1, 0)] = 0.33f * (x[GetXYZ(N,N - 2, N - 1, 0)]
		+ x[GetXYZ(N,N - 1, N - 2, 0)]
		+ x[GetXYZ(N,N - 1, N - 1, 1)]);
	x[GetXYZ(N,N - 1, 0, N - 1)] = 0.33f * (x[GetXYZ(N,N - 2, 0, N - 1)]
		+ x[GetXYZ(N,N - 1, 1, N - 1)]
		+ x[GetXYZ(N,N - 1, 0, N - 2)]);
	x[GetXYZ(N,N - 1, N - 1, N - 1)] = 0.33f * (x[GetXYZ(N,N - 2, N - 1, N - 1)]
		+ x[GetXYZ(N,N - 1, N - 2, N - 1)]
		+ x[GetXYZ(N,N - 1, N - 1, N - 2)]);
}

 void FluidSolver3D::lin_solve(int b, float *x, float *x0, float a, float c, int iter, int N)
{
	float cRecip = 1.0 / c;
	for (int k = 0; k < iter; k++) {
		for (int m = 1; m < N - 1; m++) {
			for (int j = 1; j < N - 1; j++) {
				for (int i = 1; i < N - 1; i++) {
					x[GetXYZ(N,i, j, m)] =
						(x0[GetXYZ(N,i, j, m)]
						+ a*(x[GetXYZ(N,i + 1, j, m)]
						+ x[GetXYZ(N,i - 1, j, m)]
						+ x[GetXYZ(N,i, j + 1, m)]
						+ x[GetXYZ(N,i, j - 1, m)]
						+ x[GetXYZ(N,i, j, m + 1)]
						+ x[GetXYZ(N,i, j, m - 1)]
						)) * cRecip;
				}
			}
		}
		set_bnd(b, x, N);
	}
}

 void FluidSolver3D::diffuse(int b, float *x, float *x0, float diff, float dt, int iter, int N)
{
	float a = dt * diff * (N - 2) * (N - 2);
	lin_solve(b, x, x0, a, 1 + 6 * a, iter, N);
}

 void FluidSolver3D::advect(int b, float *d, float *d0, float *velocX, float *velocY, float *velocZ, float dt, int N)
{
	float i0, i1, j0, j1, k0, k1;

	float dtx = dt * (N - 2);
	float dty = dt * (N - 2);
	float dtz = dt * (N - 2);

	float s0, s1, t0, t1, u0, u1;
	float tmp1, tmp2, tmp3, x, y, z;

	float Nfloat = N;
	float ifloat, jfloat, kfloat;
	int i, j, k;

	for (k = 1, kfloat = 1; k < N - 1; k++, kfloat++) {
		for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
			for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
				tmp1 = dtx * velocX[GetXYZ(N,i, j, k)];
				tmp2 = dty * velocY[GetXYZ(N,i, j, k)];
				tmp3 = dtz * velocZ[GetXYZ(N,i, j, k)];
				x = ifloat - tmp1;
				y = jfloat - tmp2;
				z = kfloat - tmp3;

				if (x < 0.5f) x = 0.5f;
				if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
				i0 = floorf(x);
				i1 = i0 + 1.0f;
				if (y < 0.5f) y = 0.5f;
				if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
				j0 = floorf(y);
				j1 = j0 + 1.0f;
				if (z < 0.5f) z = 0.5f;
				if (z > Nfloat + 0.5f) z = Nfloat + 0.5f;
				k0 = floorf(z);
				k1 = k0 + 1.0f;

				s1 = x - i0;
				s0 = 1.0f - s1;
				t1 = y - j0;
				t0 = 1.0f - t1;
				u1 = z - k0;
				u0 = 1.0f - u1;

				int i0i = i0;
				int i1i = i1;
				int j0i = j0;
				int j1i = j1;
				int k0i = k0;
				int k1i = k1;

				d[GetXYZ(N,i, j, k)] =

					s0 * (t0 * (u0 * d0[GetXYZ(N,i0i, j0i, k0i)]
					+ u1 * d0[GetXYZ(N,i0i, j0i, k1i)])
					+ (t1 * (u0 * d0[GetXYZ(N,i0i, j1i, k0i)]
					+ u1 * d0[GetXYZ(N,i0i, j1i, k1i)])))
					+ s1 * (t0 * (u0 * d0[GetXYZ(N,i1i, j0i, k0i)]
					+ u1 * d0[GetXYZ(N,i1i, j0i, k1i)])
					+ (t1 * (u0 * d0[GetXYZ(N,i1i, j1i, k0i)]
					+ u1 * d0[GetXYZ(N,i1i, j1i, k1i)])));
			}
		}
	}
	set_bnd(b, d, N);
}

 void FluidSolver3D::project(float *velocX, float *velocY, float *velocZ, float *p, float *div, int iter, int N)
{
	for (int k = 1; k < N - 1; k++) {
		for (int j = 1; j < N - 1; j++) {
			for (int i = 1; i < N - 1; i++) {
				div[GetXYZ(N,i, j, k)] = -0.5f*(
					velocX[GetXYZ(N,i + 1, j, k)]
					- velocX[GetXYZ(N,i - 1, j, k)]
					+ velocY[GetXYZ(N,i, j + 1, k)]
					- velocY[GetXYZ(N,i, j - 1, k)]
					+ velocZ[GetXYZ(N,i, j, k + 1)]
					- velocZ[GetXYZ(N,i, j, k - 1)]
					) / N;
				p[GetXYZ(N,i, j, k)] = 0;
			}
		}
	}
	set_bnd(0, div, N);
	set_bnd(0, p, N);
	lin_solve(0, p, div, 1, 6, iter, N);

	for (int k = 1; k < N - 1; k++) {
		for (int j = 1; j < N - 1; j++) {
			for (int i = 1; i < N - 1; i++) {
				velocX[GetXYZ(N,i, j, k)] -= 0.5f * (p[GetXYZ(N,i + 1, j, k)]
					- p[GetXYZ(N,i - 1, j, k)]) * N;
				velocY[GetXYZ(N,i, j, k)] -= 0.5f * (p[GetXYZ(N,i, j + 1, k)]
					- p[GetXYZ(N,i, j - 1, k)]) * N;
				velocZ[GetXYZ(N,i, j, k)] -= 0.5f * (p[GetXYZ(N,i, j, k + 1)]
					- p[GetXYZ(N,i, j, k - 1)]) * N;
			}
		}
	}
	set_bnd(1, velocX, N);
	set_bnd(2, velocY, N);
	set_bnd(3, velocZ, N);
}

//void FluidSolver3D::FluidSolver3D::VelocityStep(int gridSize, float* velocityX, float* prevVelX, float* velocityY, float* prevVelY, float* velocityZ, float* prevVelZ, float visc, float dt)
//{
//
//	AddSource(gridSize, velocityX, prevVelX, dt);
//	AddSource(gridSize, velocityY, prevVelY, dt);
//	std::swap(prevVelX, velocityX);
//	Diffuse(gridSize, 1, velocityX, prevVelX, visc, dt);
//	std::swap(prevVelY, velocityY);
//	Diffuse(gridSize, 2, velocityY, prevVelY, visc, dt);
//	Project(gridSize, velocityX, velocityY, prevVelX, prevVelY);
//	std::swap(prevVelX, velocityX);
//	std::swap(prevVelY, velocityY);
//	Advect(gridSize, 1, velocityX, prevVelX, prevVelX, prevVelY,prevVelZ, dt);
//	Advect(gridSize, 2, velocityY, prevVelY, prevVelX, prevVelY,prevVelZ, gridSize);
//	Project(gridSize, velocityX, velocityY, prevVelX, prevVelY);
//}
//
//void FluidSolver3D::FluidSolver3D::DensityStep(int gridSize, float* density, float* prevDensity, float* velocityX, float* velocityY, float* velocityZ,  float diffusion, float dt)
//{
//	//density
//	AddSource(gridSize, density, prevDensity, dt);
//	std::swap(prevDensity, density);
//	Diffuse(gridSize, 0, density, prevDensity, diffusion, dt);
//	std::swap(prevDensity, density);
//	Advect(gridSize, 0, density, prevDensity, velocityX, velocityY, velocityZ, dt);
//
//}
//
//
//
//
////gridSize, 0, dens, prevDens, xVel, yVel, dt
//void FluidSolver3D::FluidSolver3D::Advect(int gridSize, int b, float* currentField, float* prevField, float* xVel, float* yVel, float* zVel, float dt)
//{
//	int i, j, k, i0, j0, i1, j1, k0, k1;
//	float x, y, z, s0, s1, t0, t1, u0, u1, dt0;
//	dt0 = dt*gridSize;
//
//	for (k = 1; k <= gridSize; k++)
//	{
//		for (j = 1; j <= gridSize; j++)
//		{
//			for (i = 1; i <= gridSize; j++)
//			{
//				x = i - dt0*xVel[GetXYZ(gridSize, i, j, k)];
//				y = j - dt0*yVel[GetXYZ(gridSize, i, j, k)];
//				z = j - dt0*zVel[GetXYZ(gridSize, i, j, k)];
//
//				if (x < 0.5)
//				{
//					x = 0.5f;
//				}
//				if (x > gridSize + 0.5)
//				{
//					x = gridSize + 0.5f;
//				}
//				i0 = (int)x;
//				i1 = i0 + 1;
//				if (y < 0.5)
//				{
//					y = 0.5f;
//				}
//				if (y > gridSize + 0.5)
//				{
//					y = gridSize + 0.5f;
//				}
//				j0 = (int)y;
//				j1 = j0 + 1;
//				if (z < 0.5f) z = 0.5f;
//				if (z > gridSize + 0.5f) z = gridSize + 0.5f;
//				k0 = int(z);
//				k1 = k0 + 1.0f;
//
//				s1 = x - i0;
//				s0 = 1 - s1;
//				t1 = y - j0;
//				t0 = 1 - t1;
//				u1 = z - k0;
//				u0 = 1.0f - u1;
//
//				int i0i = i0;
//				int i1i = i1;
//				int j0i = j0;
//				int j1i = j1;
//				int k0i = k0;
//				int k1i = k1;
//
//				currentField[GetXYZ(gridSize, i, j, k)] =
//
//					s0 * (t0 * (u0 * prevField[GetXYZ(gridSize, i0i, j0i, k0i)]
//					+ u1 * prevField[GetXYZ(gridSize, i0i, j0i, k1i)])
//					+ (t1 * (u0 * prevField[GetXYZ(gridSize, i0i, j1i, k0i)]
//					+ u1 * prevField[GetXYZ(gridSize, i0i, j1i, k1i)])))
//					+ s1 * (t0 * (u0 * prevField[GetXYZ(gridSize, i1i, j0i, k0i)]
//					+ u1 * prevField[GetXYZ(gridSize, i1i, j0i, k1i)])
//					+ (t1 * (u0 * prevField[GetXYZ(gridSize, i1i, j1i, k0i)]
//					+ u1 * prevField[GetXYZ(gridSize, i1i, j1i, k1i)])));
//			}
//		}
//	}
//	SetBnds(gridSize, b, currentField);
//}
//
//void FluidSolver3D::FluidSolver3D::Project(int gridSize, float* xVel, float* yVel, float* xVelPrev, float* yVelPrev)
//{
//	/*int i, j, k;
//	float h;
//	h = 1.0f / gridSize;
//	for (i = 1; i <= gridSize; i++)
//	{
//	for (j = 1; j <= gridSize; j++)
//	{
//	yVelPrev[GetXY(gridSize, i, j)] = -0.5f * h * (xVel[GetXY(gridSize, i + 1, j)] - xVel[GetXY(gridSize, i - 1, j)] +
//	yVel[GetXY(gridSize, i, j + 1)] - yVel[GetXY(gridSize, i, j - 1)]);
//	xVelPrev[GetXY(gridSize, i, j)] = 0;
//	}
//	}
//	SetBnds(gridSize, 0, yVelPrev); SetBnds(gridSize, 0, xVelPrev);
//	for (k = 0; k < 20; k++)
//	{
//	for (i = 1; i <= gridSize; i++)
//	{
//	for (j = 1; j <= gridSize; j++)
//	{
//	xVelPrev[GetXY(gridSize, i, j)] = (yVelPrev[GetXY(gridSize, i, j)] + xVelPrev[GetXY(gridSize, i - 1, j)] + xVelPrev[GetXY(gridSize, i + 1, j)] +
//	xVelPrev[GetXY(gridSize, i, j - 1)] + xVelPrev[GetXY(gridSize, i, j + 1)]) / 4;
//	}
//	}
//	SetBnds(gridSize, 0, xVelPrev);
//	}
//	for (i = 1; i <= gridSize; i++)
//	{
//	for (j = 1; j <= gridSize; j++)
//	{
//	xVel[GetXY(gridSize, i, j)] -= 0.5f * (xVelPrev[GetXY(gridSize, i + 1, j)] - xVelPrev[GetXY(gridSize, i - 1, j)]) / h;
//	yVel[GetXY(gridSize, i, j)] -= 0.5f * (xVelPrev[GetXY(gridSize, i, j + 1)] - xVelPrev[GetXY(gridSize, i, j - 1)]) / h;
//	}
//	}
//	SetBnds(gridSize, 1, xVel); SetBnds(gridSize, 2, yVel);*/
//}
//
//void FluidSolver3D::FluidSolver3D::SetBnds(int gridSize, int b, float* field)
//{
//
//	for (int j = 1; j < gridSize - 1; j++) {
//		for (int i = 1; i < gridSize - 1; i++) {
//			field[GetXYZ(gridSize,i, j, 0)] = b == 3 ? -field[GetXYZ(gridSize,i, j, 1)] : field[GetXYZ(gridSize,i, j, 1)];
//			field[GetXYZ(gridSize, i, j, gridSize - 1)] = b == 3 ? -field[GetXYZ(gridSize, i, j, gridSize - 2)] : field[GetXYZ(gridSize, i, j, gridSize - 2)];
//		}
//	}
//	for (int k = 1; k < gridSize - 1; k++) {
//		for (int i = 1; i < gridSize - 1; i++) {
//			field[GetXYZ(gridSize,i, 0, k)] = b == 2 ? -field[GetXYZ(gridSize,i, 1, k)] : field[GetXYZ(gridSize,i, 1, k)];
//			field[GetXYZ(gridSize,i, gridSize - 1, k)] = b == 2 ? -field[GetXYZ(gridSize,i, gridSize - 2, k)] : field[GetXYZ(gridSize,i, gridSize - 2, k)];
//		}
//	}
//	for (int k = 1; k < gridSize - 1; k++) {
//		for (int j = 1; j < gridSize - 1; j++) {
//			field[GetXYZ(gridSize,0, j, k)] = b == 1 ? -field[GetXYZ(gridSize,1, j, k)] : field[GetXYZ(gridSize,1, j, k)];
//			field[GetXYZ(gridSize,gridSize - 1, j, k)] = b == 1 ? -field[GetXYZ(gridSize,gridSize - 2, j, k)] : field[GetXYZ(gridSize,gridSize - 2, j, k)];
//		}
//	}
//
//	field[GetXYZ(gridSize,0, 0, 0)] = 0.33f * (field[GetXYZ(gridSize,1, 0, 0)]
//		+ field[GetXYZ(gridSize,0, 1, 0)]
//		+ field[GetXYZ(gridSize,0, 0, 1)]);
//	field[GetXYZ(gridSize,0, gridSize - 1, 0)] = 0.33f * (field[GetXYZ(gridSize,1, gridSize - 1, 0)]
//		+ field[GetXYZ(gridSize,0, gridSize - 2, 0)]
//		+ field[GetXYZ(gridSize,0, gridSize - 1, 1)]);
//	field[GetXYZ(gridSize,0, 0, gridSize - 1)] = 0.33f * (field[GetXYZ(gridSize,1, 0, gridSize - 1)]
//		+ field[GetXYZ(gridSize,0, 1, gridSize - 1)]
//		+ field[GetXYZ(gridSize,0, 0, gridSize)]);
//	field[GetXYZ(gridSize,0, gridSize - 1, gridSize - 1)] = 0.33f * (field[GetXYZ(gridSize,1, gridSize - 1, gridSize - 1)]
//		+ field[GetXYZ(gridSize,0, gridSize - 2, gridSize - 1)]
//		+ field[GetXYZ(gridSize,0, gridSize - 1, gridSize - 2)]);
//	field[GetXYZ(gridSize,gridSize - 1, 0, 0)] = 0.33f * (field[GetXYZ(gridSize,gridSize - 2, 0, 0)]
//		+ field[GetXYZ(gridSize,gridSize - 1, 1, 0)]
//		+ field[GetXYZ(gridSize,gridSize - 1, 0, 1)]);
//	field[GetXYZ(gridSize,gridSize - 1, gridSize - 1, 0)] = 0.33f * (field[GetXYZ(gridSize,gridSize - 2, gridSize - 1, 0)]
//		+ field[GetXYZ(gridSize,gridSize - 1, gridSize - 2, 0)]
//		+ field[GetXYZ(gridSize,gridSize - 1, gridSize - 1, 1)]);
//	field[GetXYZ(gridSize,gridSize - 1, 0, gridSize - 1)] = 0.33f * (field[GetXYZ(gridSize,gridSize - 2, 0, gridSize - 1)]
//		+ field[GetXYZ(gridSize,gridSize - 1, 1, gridSize - 1)]
//		+ field[GetXYZ(gridSize,gridSize - 1, 0, gridSize - 2)]);
//	field[GetXYZ(gridSize,gridSize - 1, gridSize - 1, gridSize - 1)] = 0.33f * (field[GetXYZ(gridSize,gridSize - 2, gridSize - 1, gridSize - 1)]
//		+ field[GetXYZ(gridSize,gridSize - 1, gridSize - 2, gridSize - 1)]
//		+ field[GetXYZ(gridSize,gridSize - 1, gridSize - 1, gridSize - 2)]);
//}
//
//void FluidSolver3D::FluidSolver3D::AddSource(int gridSize, float* currentSource, float* prevSource, float dt)
//{
//	int i, size = (gridSize + 2) * (gridSize + 2);
//	for (i = 0; i < size; i++)
//	{
//		float x = prevSource[i];
//		currentSource[i] += dt * prevSource[i];
//	}
//}
//
//void FluidSolver3D::FluidSolver3D::Diffuse(int gridSize, int b, float* dens, float* prevDens, float diff, float dt)
//{
//	float diffusionRate = dt * diff * gridSize * gridSize;
//	int x, y, z, k;
//
//	for (k = 0; k < 20; k++)
//	{
//		for (x = 1; x <= gridSize; x++)
//		{
//			for (y = 1; y <= gridSize; y++)
//			{
//				for (z = 1; z <= gridSize; z++)
//				{
//
//					dens[GetXYZ(gridSize, x, y,z)] =
//						(prevDens[GetXYZ(gridSize, x, y,z)]
//						+ diffusionRate *(dens[GetXYZ(gridSize, x - 1, y, z)] 
//										+ dens[GetXYZ(gridSize, x + 1, y, z)] 
//										+ dens[GetXYZ(gridSize, x, y - 1, z)] 
//										+ dens[GetXYZ(gridSize, x, y + 1, z)]
//										+ dens[GetXYZ(gridSize, x, y, z - 1)]
//										+ dens[GetXYZ(gridSize, x, y, z + 1)]
//										)) / (1 + 4 * diffusionRate);
//				}
//
//			}
//		}
//	}
//
//	SetBnds(gridSize, b, dens);
//}

