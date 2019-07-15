#pragma once

#include "Window.h"
#include "Camera.h"
#include "DrawEngine.h"
#include "Controller.h"

class IEngine
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
	IObject* m_object;
	// DeltaTime
	float m_deltaTime;


public:
	IEngine() {};
	~IEngine();

	// Initialise code.
	virtual void Initialise() = 0;

	void MainLoop();

};

