#pragma once
#if GL

#include "Engine.h"
#include "GLDrawEngine.h"

class GLEngine : 
	public Engine
{
private:
	// GLDraw Engine
	GLDrawEngine* m_myDrawEngine;

	// OpenGL Engine Init Code.
	void Initialise();
	// Vulkan Engine Main Loop.
	void MainLoop();
public:
	GLEngine();
	~GLEngine() {};
};

#endif