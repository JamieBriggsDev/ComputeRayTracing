#pragma once

#include <vector>

#include "Window.h"
#include "Controller.h"
#include "Shapes.h"

#define VULKAN_CLEAR_COLOUR { 0.92f, 0.6f, 0.6f, 0.0f }
#define OPENGL_CLEAR_COLOUR 0.6f, 0.85f, 0.92f, 0.0f

class Engine
{
protected:
	// Window object
	Window* m_myWindow;
	// Controller Component
	Controller* m_myController;
	// Spheres
	std::vector<Sphere> m_spheres;
	// Planes
	std::vector<Plane> m_planes;
	// DeltaTime
	float m_deltaTime;
	// Total Time
	float m_totalTime = 0.0f;


	// Initialise objects.
	void InitialiseObjects(std::vector<Plane> &_planes, std::vector<Sphere> &_spheres);

public:
	Engine();
	~Engine();
	// Pre Rendered Frames
	static int sPreRenderedFrames;
	// Update
	void Update();

	// Initialise code.
	virtual void Initialise() = 0;
	// Main Loop
	virtual void MainLoop() = 0;

};

