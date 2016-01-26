#include "Debug.h"
#include <iostream>
#include <string>

Debug *Debug::m_Debug = nullptr;

Debug::Debug(){}
Debug::~Debug(){}

void* Debug::operator new(size_t memorySize)
{
	return _aligned_malloc(memorySize, 16);

}

void Debug::operator delete(void* memoryBlockPtr)
{
	_aligned_free(memoryBlockPtr);
}

void Debug::Log(void* _input)
{
	std::cout << _input << std::endl;
}