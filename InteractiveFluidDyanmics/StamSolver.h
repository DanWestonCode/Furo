/// <summary>
/// StamSolver.h
///
/// About:
/// StamSolver.h is the base class for both the 2D and 3D CPU 
/// methods within Furo. The class is utilized as a base class,
/// containing variables common between the two simulations
/// </summary>
#ifndef StamSolver_h__
#define StamSolver_h__

#include "FluidBase.h"
namespace Furo
{
	class StamSolver : public FluidBase
	{
	public:
		virtual void Initialize();
		virtual void Run(float){};
		virtual void Shutdown();
		virtual void Clear(){};
	public:
		//Getters/Setters
		int GetGridSize(){ return m_gridSize; }
		void SetGridSize(int s){ m_gridSize = s; }

		float GetDiffusion(){ return m_diffusion; }
		void SetDiffusion(float d){ m_diffusion = d; }

		float GetVisc(){ return m_visc; }
		void SetVisc(float v){ m_visc = v; }

		float GetDt(){ return m_dt; }
		void SetDt(float d){ m_dt = d; }

		float* GetDensity(){ return m_density; }
		virtual void SetDensity(){};
		virtual void SetVelX(){};
		virtual void SetVelY(){};
		virtual void SetVelZ(){};

		float m_diffusion, m_visc, m_dt;

		//Variables
	public:
		int m_gridSize;

		float* m_velocityX;
		float* m_velocityY;

		float* m_prevVelX;
		float* m_prevVelY;

		float* m_density;
		float* m_prevDensity;
	};
}
#endif // StamSolver_h__