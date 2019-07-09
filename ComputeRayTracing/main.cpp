// JBEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

// Console input
#include <stdio.h>
#include <stdlib.h>


// Window
#include "Engine.h"


int main()
{
    std::cout << "Welcome to the JBEngine!\n"; 

	// Initialise Engine
	Engine* JBEngine = new Engine();
	// Start main loop
	JBEngine->MainLoop();

	return 0;
}
