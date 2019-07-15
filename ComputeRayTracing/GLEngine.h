#pragma once
#if GL

#include "IEngine.h"

class GLEngine : 
	public IEngine
{
private:
	// OpenGL Engine Init Code;
	void Initialise();
public:
	GLEngine();
	~GLEngine() {};
};

#endif