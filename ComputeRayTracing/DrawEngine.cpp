#include "DrawEngine.h"
#include "Camera.h"
#include "Window.h"

// GLEW before everything else
#include <GL/glew.h>

// GLFW to handle window and keyboard/ mouse input
#include <GLFW/glfw3.h>


void DrawEngine::Update(Camera* _camera, Window* _window, Object* _object)
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Draw Object
	_object->Draw(_camera);


	// Swap buffers
	glfwSwapBuffers(_window->GetWindowComponent());
	glfwPollEvents();
}

DrawEngine::DrawEngine()
{
}


DrawEngine::~DrawEngine()
{
}
