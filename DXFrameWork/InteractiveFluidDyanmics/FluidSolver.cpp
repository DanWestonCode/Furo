#include "FluidSolver.h"
#include "FluidHelper.h"
#include <algorithm>    
#define GetXY(N,i,j) ((i)+(N + 2)*(j))

FluidSolver::FluidSolver()
{

}

FluidSolver::~FluidSolver()
{

}

void FluidSolver::VelocityStep(int gridSize, float* velocityX, float* prevVelX, float* velocityY, float* prevVelY, float visc, float dt)
{

	AddSource(gridSize, velocityX, prevVelX, dt);
	AddSource(gridSize, velocityY, prevVelY, dt);
	std::swap(prevVelX, velocityX);
	Diffuse(gridSize, 1, velocityX, prevVelX, visc, dt);
	std::swap(prevVelY, velocityY);
	Diffuse(gridSize, 2, velocityY, prevVelY, visc, dt);
	Project(gridSize, velocityX, velocityY, prevVelX, prevVelY);
	std::swap(prevVelX, velocityX);
	std::swap(prevVelY, velocityY);
	Advect(gridSize, 1, velocityX, prevVelX, prevVelX, prevVelY, dt);
	Advect(gridSize, 2, velocityY, prevVelY, prevVelX, prevVelY, gridSize);
	Project(gridSize, velocityX, velocityY, prevVelX, prevVelY);
}

void FluidSolver::DensityStep(int gridSize, float* density, float* prevDensity, float* velocityX, float* velocityY, float diffusion, float dt)
{
	//density
	AddSource(gridSize, density, prevDensity, dt);
	std::swap(prevDensity, density);
	Diffuse(gridSize, 0, density, prevDensity, diffusion, dt);
	std::swap(prevDensity, density);
	Advect(gridSize, 0, density, prevDensity, velocityX, velocityY, dt);

}

//gridSize, 0, dens, prevDens, xVel, yVel, dt
void FluidSolver::Advect(int gridSize, int b, float* currentField, float* prevField, float* xVel, float* yVel, float dt)
{
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;
	dt0 = dt*gridSize;
	for (i = 1; i <= gridSize; i++)
	{
		for (j = 1; j <= gridSize; j++)
		{
			x = i - dt0*xVel[GetXY(gridSize, i, j)];
			y = j - dt0*yVel[GetXY(gridSize, i, j)];

			if (x < 0.5)
			{
				x = 0.5f;
			}
			if (x > gridSize + 0.5)
			{
				x = gridSize + 0.5f;
			}
			i0 = (int)x;
			i1 = i0 + 1;
			if (y < 0.5)
			{
				y = 0.5f;
			}
			if (y > gridSize + 0.5)
			{
				y = gridSize + 0.5f;
			}
			j0 = (int)y;
			j1 = j0 + 1;
			s1 = x - i0;
			s0 = 1 - s1;
			t1 = y - j0;
			t0 = 1 - t1;

			currentField[GetXY(gridSize, i, j)] = s0 * (t0 * prevField[GetXY(gridSize, i0, j0)] + t1 * prevField[GetXY(gridSize, i0, j1)]) +
				s1*(t0*prevField[GetXY(gridSize, i1, j0)] + t1*prevField[GetXY(gridSize, i1, j1)]);
			
		}
	}
	SetBnds(gridSize, b, currentField);
}

void FluidSolver::Project(int gridSize, float* xVel, float* yVel, float* xVelPrev, float* yVelPrev)
{
	int i, j, k;
	float h;
	h = 1.0f / gridSize;
	for (i = 1; i <= gridSize; i++)
	{
		for (j = 1; j <= gridSize; j++)
		{
			yVelPrev[GetXY(gridSize, i, j)] = -0.5f * h * (xVel[GetXY(gridSize, i + 1, j)] - xVel[GetXY(gridSize, i - 1, j)] +
				yVel[GetXY(gridSize, i, j + 1)] - yVel[GetXY(gridSize, i, j - 1)]);
			xVelPrev[GetXY(gridSize, i, j)] = 0;
		}
	}
	SetBnds(gridSize, 0, yVelPrev); SetBnds(gridSize, 0, xVelPrev);
	for (k = 0; k < 20; k++)
	{
		for (i = 1; i <= gridSize; i++)
		{
			for (j = 1; j <= gridSize; j++)
			{
				xVelPrev[GetXY(gridSize, i, j)] = (yVelPrev[GetXY(gridSize, i, j)] + xVelPrev[GetXY(gridSize, i - 1, j)] + xVelPrev[GetXY(gridSize, i + 1, j)] +
					xVelPrev[GetXY(gridSize, i, j - 1)] + xVelPrev[GetXY(gridSize, i, j + 1)]) / 4;
			}
		}
		SetBnds(gridSize, 0, xVelPrev);
	}
	for (i = 1; i <= gridSize; i++)
	{
		for (j = 1; j <= gridSize; j++)
		{
			xVel[GetXY(gridSize, i, j)] -= 0.5f * (xVelPrev[GetXY(gridSize, i + 1, j)] - xVelPrev[GetXY(gridSize, i - 1, j)]) / h;
			yVel[GetXY(gridSize, i, j)] -= 0.5f * (xVelPrev[GetXY(gridSize, i, j + 1)] - xVelPrev[GetXY(gridSize, i, j - 1)]) / h;
		}
	}
	SetBnds(gridSize, 1, xVel); SetBnds(gridSize, 2, yVel);
}

void FluidSolver::SetBnds(int gridSize, int b, float* field)
{

	int i;
	for (i = 1; i <= gridSize; i++)
	{
		field[GetXY(gridSize, 0, i)] = b == 1 ? -field[GetXY(gridSize, 1, i)] : field[GetXY(gridSize, 1, i)];
		field[GetXY(gridSize, gridSize + 1, i)] = b == 1 ? -field[GetXY(gridSize, gridSize, i)] : field[GetXY(gridSize, gridSize, i)];
		field[GetXY(gridSize, i, 0)] = b == 2 ? -field[GetXY(gridSize, i, 1)] : field[GetXY(gridSize, i, 1)];
		field[GetXY(gridSize, i, gridSize + 1)] = b == 2 ? -field[GetXY(gridSize, i, gridSize)] : field[GetXY(gridSize, i, gridSize)];
	}

	field[GetXY(gridSize, 0, 0)] = 0.5f*(field[GetXY(gridSize, 1, 0)] + field[GetXY(gridSize, 0, 1)]);
	field[GetXY(gridSize, 0, gridSize + 1)] = 0.5f*(field[GetXY(gridSize, 1, gridSize + 1)] + field[GetXY(gridSize, 0, gridSize)]);
	field[GetXY(gridSize, gridSize + 1, 0)] = 0.5f*(field[GetXY(gridSize, gridSize, 0)] + field[GetXY(gridSize, gridSize + 1, 1)]);
	field[GetXY(gridSize, gridSize + 1, gridSize + 1)] = 0.5f*(field[GetXY(gridSize, gridSize, gridSize + 1)] + field[GetXY(gridSize, gridSize + 1, gridSize)]);
}

void FluidSolver::AddSource(int gridSize, float* currentSource, float* prevSource, float dt)
{
	int i, size = (gridSize + 2) * (gridSize + 2);
	for (i = 0; i < size; i++)
	{
		float x = prevSource[i];
		currentSource[i] += dt * prevSource[i];
	}
}

void FluidSolver::Diffuse(int gridSize, int b, float* dens, float* prevDens, float diff, float dt)
{
	float diffusionRate = dt * diff * gridSize * gridSize;
	int x, y, k;

	for (k = 0; k < 20; k++)
	{
		for (x = 1; x <= gridSize; x++)
		{
			for (y = 1; y <= gridSize; y++)
			{
				dens[GetXY(gridSize, x, y)] =
					(prevDens[GetXY(gridSize, x, y)] + diffusionRate * (dens[GetXY(gridSize, x - 1, y)] +
					dens[GetXY(gridSize, x + 1, y)] + dens[GetXY(gridSize, x, y - 1)] +
					dens[GetXY(gridSize, x, y + 1)])) / (1 + 4 * diffusionRate);

			}
		}
	}

	SetBnds(gridSize, b, dens);
}