/*************************************************************/
/*                                                           */
/* Fluid is the base class for all fluid simulations created */
/* using Furo. The fluid base class contains base member var */
/* -iables which are shared between both 3D and 2D based sim */
/*                                                           */
/* Created by Daniel Weston 19/12/2015                       */
/*************************************************************/
#ifndef _FLUID_H_
#define _FLUID_H_

class Fluid
{
//Functions
public:
	Fluid();
	~Fluid();

	void Initialize(int,float);
	void Initialize(int, float, float, float);
	void Shutdown();

//Getters/Setters
public:
	int GetGridSize(){ return m_gridSize; }
	void SetGridSize(int s){ m_gridSize = s; }

	float GetDiffusion(){ return m_diffusion; }
	void SetDiffusion(float d){ m_diffusion = d; }

	float GetVisc(){ return m_visc; }
	void SetVisc(float v){ m_visc = v; }

	float GetDt(){ return m_dt; }
	void SetDt(float d){ m_dt = d; }

//Variables
private:
	int m_gridSize;
	float m_diffusion, m_visc, m_dt;

	float* m_velocityX;
	float* m_velocityY;
	float* m_prevVelX;
	float* m_prevVelY;
	float* m_density;
	float* m_prevDensity;
};

#endif