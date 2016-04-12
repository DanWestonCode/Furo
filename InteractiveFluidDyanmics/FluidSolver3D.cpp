/// <summary>
/// FluidSolver3D.cpp
///
/// About:
/// FluidSolver3D.cpp is a 3D adaptation of Stams solver seem within 
/// FluidSolver2D.cpp. The implementation contained within this 
/// class was guided by Mike Ash's article'Fluid Simulation for Dummies'
///
/// References:
/// All functions guided by Mike Ash - https://mikeash.com/pyblog/fluid-simulation-for-dummies.html
/// </summary>
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
			x[GetXYZ(N, i, j, 0)] = b == 3 ? -x[GetXYZ(N, i, j, 1)] : x[GetXYZ(N, i, j, 1)];
			x[GetXYZ(N, i, j, N - 1)] = b == 3 ? -x[GetXYZ(N, i, j, N - 2)] : x[GetXYZ(N, i, j, N - 2)];
		}
	}
	for (int k = 1; k < N - 1; k++) {
		for (int i = 1; i < N - 1; i++) {
			x[GetXYZ(N, i, 0, k)] = b == 2 ? -x[GetXYZ(N, i, 1, k)] : x[GetXYZ(N, i, 1, k)];
			x[GetXYZ(N, i, N - 1, k)] = b == 2 ? -x[GetXYZ(N, i, N - 2, k)] : x[GetXYZ(N, i, N - 2, k)];
		}
	}
	for (int k = 1; k < N - 1; k++) {
		for (int j = 1; j < N - 1; j++) {
			x[GetXYZ(N, 0, j, k)] = b == 1 ? -x[GetXYZ(N, 1, j, k)] : x[GetXYZ(N, 1, j, k)];
			x[GetXYZ(N, N - 1, j, k)] = b == 1 ? -x[GetXYZ(N, N - 2, j, k)] : x[GetXYZ(N, N - 2, j, k)];
		}
	}

	x[GetXYZ(N, 0, 0, 0)] = 0.33f * (x[GetXYZ(N, 1, 0, 0)]
		+ x[GetXYZ(N, 0, 1, 0)]
		+ x[GetXYZ(N, 0, 0, 1)]);
	x[GetXYZ(N, 0, N - 1, 0)] = 0.33f * (x[GetXYZ(N, 1, N - 1, 0)]
		+ x[GetXYZ(N, 0, N - 2, 0)]
		+ x[GetXYZ(N, 0, N - 1, 1)]);
	x[GetXYZ(N, 0, 0, N - 1)] = 0.33f * (x[GetXYZ(N, 1, 0, N - 1)]
		+ x[GetXYZ(N, 0, 1, N - 1)]
		+ x[GetXYZ(N, 0, 0, N)]);
	x[GetXYZ(N, 0, N - 1, N - 1)] = 0.33f * (x[GetXYZ(N, 1, N - 1, N - 1)]
		+ x[GetXYZ(N, 0, N - 2, N - 1)]
		+ x[GetXYZ(N, 0, N - 1, N - 2)]);
	x[GetXYZ(N, N - 1, 0, 0)] = 0.33f * (x[GetXYZ(N, N - 2, 0, 0)]
		+ x[GetXYZ(N, N - 1, 1, 0)]
		+ x[GetXYZ(N, N - 1, 0, 1)]);
	x[GetXYZ(N, N - 1, N - 1, 0)] = 0.33f * (x[GetXYZ(N, N - 2, N - 1, 0)]
		+ x[GetXYZ(N, N - 1, N - 2, 0)]
		+ x[GetXYZ(N, N - 1, N - 1, 1)]);
	x[GetXYZ(N, N - 1, 0, N - 1)] = 0.33f * (x[GetXYZ(N, N - 2, 0, N - 1)]
		+ x[GetXYZ(N, N - 1, 1, N - 1)]
		+ x[GetXYZ(N, N - 1, 0, N - 2)]);
	x[GetXYZ(N, N - 1, N - 1, N - 1)] = 0.33f * (x[GetXYZ(N, N - 2, N - 1, N - 1)]
		+ x[GetXYZ(N, N - 1, N - 2, N - 1)]
		+ x[GetXYZ(N, N - 1, N - 1, N - 2)]);
}

void FluidSolver3D::lin_solve(int b, float *x, float *x0, float a, float c, int iter, int N)
{
	float cRecip = 1.0 / c;
	for (int k = 0; k < iter; k++) {
		for (int m = 1; m < N - 1; m++) {
			for (int j = 1; j < N - 1; j++) {
				for (int i = 1; i < N - 1; i++) {
					x[GetXYZ(N, i, j, m)] =
						(x0[GetXYZ(N, i, j, m)]
						+ a*(x[GetXYZ(N, i + 1, j, m)]
						+ x[GetXYZ(N, i - 1, j, m)]
						+ x[GetXYZ(N, i, j + 1, m)]
						+ x[GetXYZ(N, i, j - 1, m)]
						+ x[GetXYZ(N, i, j, m + 1)]
						+ x[GetXYZ(N, i, j, m - 1)]
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
				tmp1 = dtx * velocX[GetXYZ(N, i, j, k)];
				tmp2 = dty * velocY[GetXYZ(N, i, j, k)];
				tmp3 = dtz * velocZ[GetXYZ(N, i, j, k)];
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

				d[GetXYZ(N, i, j, k)] =

					s0 * (t0 * (u0 * d0[GetXYZ(N, i0i, j0i, k0i)]
					+ u1 * d0[GetXYZ(N, i0i, j0i, k1i)])
					+ (t1 * (u0 * d0[GetXYZ(N, i0i, j1i, k0i)]
					+ u1 * d0[GetXYZ(N, i0i, j1i, k1i)])))
					+ s1 * (t0 * (u0 * d0[GetXYZ(N, i1i, j0i, k0i)]
					+ u1 * d0[GetXYZ(N, i1i, j0i, k1i)])
					+ (t1 * (u0 * d0[GetXYZ(N, i1i, j1i, k0i)]
					+ u1 * d0[GetXYZ(N, i1i, j1i, k1i)])));
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
				div[GetXYZ(N, i, j, k)] = -0.5f*(
					velocX[GetXYZ(N, i + 1, j, k)]
					- velocX[GetXYZ(N, i - 1, j, k)]
					+ velocY[GetXYZ(N, i, j + 1, k)]
					- velocY[GetXYZ(N, i, j - 1, k)]
					+ velocZ[GetXYZ(N, i, j, k + 1)]
					- velocZ[GetXYZ(N, i, j, k - 1)]
					) / N;
				p[GetXYZ(N, i, j, k)] = 0;
			}
		}
	}
	set_bnd(0, div, N);
	set_bnd(0, p, N);
	lin_solve(0, p, div, 1, 6, iter, N);

	for (int k = 1; k < N - 1; k++) {
		for (int j = 1; j < N - 1; j++) {
			for (int i = 1; i < N - 1; i++) {
				velocX[GetXYZ(N, i, j, k)] -= 0.5f * (p[GetXYZ(N, i + 1, j, k)]
					- p[GetXYZ(N, i - 1, j, k)]) * N;
				velocY[GetXYZ(N, i, j, k)] -= 0.5f * (p[GetXYZ(N, i, j + 1, k)]
					- p[GetXYZ(N, i, j - 1, k)]) * N;
				velocZ[GetXYZ(N, i, j, k)] -= 0.5f * (p[GetXYZ(N, i, j, k + 1)]
					- p[GetXYZ(N, i, j, k - 1)]) * N;
			}
		}
	}
	set_bnd(1, velocX, N);
	set_bnd(2, velocY, N);
	set_bnd(3, velocZ, N);
}