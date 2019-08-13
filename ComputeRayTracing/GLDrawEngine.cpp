#if GL

#include "GLDrawEngine.h"
#include "Camera.h"
#include "Window.h"

// GLEW before everything else
#include <GL/glew.h>

// GLFW to handle window and keyboard/ mouse input
#include <GLFW/glfw3.h>



void GLDrawEngine::Update(Camera* _camera, Window* _window, Object* _object, float _deltaTime)
{

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw Object
	_object->Draw(_camera, _deltaTime);
}

GLDrawEngine::GLDrawEngine()
{
	// light blue background
	glClearColor(0.6f, 0.85f, 0.92f, 0.0f);

	// Enable face culling
	glEnable(GL_CULL_FACE);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// V sync off 
	glfwSwapInterval(0);
}


GLDrawEngine::~GLDrawEngine()
{
}

#endif