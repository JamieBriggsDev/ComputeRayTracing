#pragma once

#include <vector>

#include "Window.h"
#include "Camera.h"
#include "Controller.h"
#include "Sphere.h"
#include "Shapes.h"

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
	glm::vec3 m_light;
	std::vector<Sphere> m_spheres;
	// DeltaTime
	float m_deltaTime;


	// Initialise objects.
	void InitialiseObjects(std::vector<Plane> &_planes, std::vector<Sphere> &_spheres);


public:
	Engine();
	~Engine();

	// Initialise code.
	virtual void Initialise() = 0;

	virtual void MainLoop() = 0;

};

