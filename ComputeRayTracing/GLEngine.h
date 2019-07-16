#pragma once
#if GL

#include "Engine.h"

class GLEngine : 
	public Engine
{
private:
	// OpenGL Engine Init Code;
	void Initialise();
public:
	GLEngine();
	~GLEngine() {};
};

#endif