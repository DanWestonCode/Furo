/// <summary>
/// Debug.cpp
///
/// About:
/// Debug.cpp is a singleton intended for use when running the engine in 
/// debug mode. The class contains one function 'Log' which prints
/// messages to the console window
/// </summary>
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

void Debug::Log(char* _input)
{
	std::cout << _input << std::endl;
}