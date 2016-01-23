#include "Furo.h"

Furo::Furo()
{
	//initialize pointers 
	m_textureFluid = nullptr;
	m_volumeFluid = nullptr;
}

Furo::~Furo()
{
}

void Furo::Initialize(FluidField _type, int size, float dt)
{
	m_simulationType = _type;
	switch (_type)
	{
	case TwoDimensional:
		m_textureFluid = new FluidTwoDimensional;
		m_textureFluid->Initialize(size);
		break;
	case ThreeDimensional:
		m_volumeFluid = new FluidThreeDimensional;
		m_volumeFluid->Initialize(size);
		break;
	default:
		break;
	}
}

void Furo::Run(float dt)
{
	switch (m_simulationType)
	{
	case TwoDimensional:
		m_textureFluid->Update(dt);
		break;
	case ThreeDimensional:
		m_volumeFluid->Update(dt);
		break;
	default:
		break;
	}
}

void Furo::Shutdown()
{
	if (m_textureFluid)
	{
		m_textureFluid->Shutdown();
		delete m_textureFluid;
		m_textureFluid = nullptr;
	}

	if (m_volumeFluid)
	{
		m_volumeFluid->Shutdown();
		delete m_volumeFluid;
		m_volumeFluid = nullptr;
	}
}

Fluid* Furo::GetFluid()
{
	switch (m_simulationType)
	{
	case Furo::TwoDimensional:
		return m_textureFluid;
		break;
	case Furo::ThreeDimensional:
		return m_volumeFluid;
		break;
	default:
		return nullptr;
		break;
	}
}