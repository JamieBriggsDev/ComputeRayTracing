// JBEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

// Console input
#include <stdio.h>
#include <stdlib.h>


// Window
#if GL
#include "GLEngine.h"
#elif VK
#include "VKEngine.h"
#endif



int main()
{
    std::cout << "Welcome to the JBEngine!\n"; 

	// Initialise Engine based on API
#if GL
	Engine* JBEngine = new GLEngine();
#elif VK
	Engine* JBEngine = new VKEngine();
#endif
	// Start main loop
	JBEngine->MainLoop();

	return 0;
}
