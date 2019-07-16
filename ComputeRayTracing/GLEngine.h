#pragma once
#if GL

#include "Engine.h"

class GLEngine : 
	public Engine
{
private:
	// OpenGL Engine Init Code.
	void Initialise();
	// Vulkan Engine Main Loop.
	void MainLoop();
public:
	GLEngine();
	~GLEngine() {};
};

#endif