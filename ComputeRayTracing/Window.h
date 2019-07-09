#pragma once
// Window Resolution
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// Console input
#include <stdio.h>
#include <stdlib.h>

// GLEW before everything else
#include <GL/glew.h>

// GLFW to handle window and keyboard/ mouse input
#include <GLFW/glfw3.h>

// Include GLM for the maths
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Object Class
#include "Object.h"

// Camera class
class Camera;
// Controller Class
class Controller;

// Window class
class Window
{
private:
	// Window component
	GLFWwindow* m_window;
	// Last Time
	float LastTime;
	

	int Initialise();
public:
	Window();
	~Window();
	// Window Update Function.
	void Update();
	// Get window component.
	GLFWwindow* GetWindowComponent();
	// Check if the window is closing.
	int CheckWindowClose(Controller* _controller);

	// Static Resolution
	static int s_windowWidth;
	static int s_windowHeight;
};

