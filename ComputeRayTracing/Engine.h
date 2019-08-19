#pragma once

#include "Window.h"
#include "Camera.h"
#include "Controller.h"

#define VULKAN_CLEAR_COLOUR { 0.92f, 0.6f, 0.6f, 0.0f }
#define OPENGL_CLEAR_COLOUR 0.6f, 0.85f, 0.92f, 0.0f

class Engine
{
protected:
	// Window object
	Window* m_myWindow;
	// Camera Object
	Camera* m_myCamera;
	// Controller Component
	Controller* m_myController;
	// Objects
	Object* m_object;
	// DeltaTime
	float m_deltaTime;


public:
	Engine() {};
	~Engine();

	// Initialise code.
	virtual void Initialise() = 0;

	virtual void MainLoop() = 0;

};

