#include "FluidHelper.h"
#include <algorithm>
FluidHelper::FluidHelper()
{
}

FluidHelper::~FluidHelper()
{
}

int FluidHelper::GetIndex(int _gridSize, int _x, int _y)
{
	return (_x * (_gridSize + 2)) + _y;
}

void FluidHelper::Swap(float* _x, float* _y)
{
	//std::swap(_x, _y);
	float* temp = _x;

	_x = _y;
	_y = temp;
}

