#pragma once

#include "Window.h"
#include "Camera.h"
#include "DrawEngine.h"
#include "Controller.h"

class Engine
{
protected:
	// Window object
	Window* m_myWindow;
	// Camera Object
	Camera* m_myCamera;
	// Controller Component
	Controller* m_myController;
	// Draw Engine
	DrawEngine* m_myDrawEngine;
	// Objects
	Object* m_object;
	// DeltaTime
	float m_deltaTime;


public:
	Engine() {};
	~Engine();

	// Initialise code.
	virtual void Initialise() = 0;

	void MainLoop();

};

