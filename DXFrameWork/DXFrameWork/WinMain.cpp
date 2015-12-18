/***************************************/
/* WinMain acts as the entry point     */
/* to the program                      */
/* Created by Daniel Weston 18/12/2015 */
/***************************************/
#include "System.h"

//First function called from the framework
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	//Create a new system class 
	System* m_system;
	bool result;

	// Create the system object.
	m_system = new System;
	if (!m_system)
	{
		//if system could not be created
		//end program
		return 0;
	}

	// Initialize and run the system object.
	result = m_system->Initialize();
	if (result)
	{
		m_system->Run();
	}

	// Shutdown and release the system object.
	m_system->Shutdown();
	delete m_system;
	m_system = 0;

	return 0;
}