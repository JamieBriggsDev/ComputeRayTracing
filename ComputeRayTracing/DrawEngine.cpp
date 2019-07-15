#include "DrawEngine.h"
#include "Camera.h"
#include "Window.h"

// GLEW before everything else
#include <GL/glew.h>

// GLFW to handle window and keyboard/ mouse input
#include <GLFW/glfw3.h>

#if VK
void CreateGraphicsPipeline()
{

}

#endif


void DrawEngine::Update(Camera* _camera, Window* _window, IObject* _object)
{

#if GL
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#endif
	// Draw Object
	_object->Draw(_camera);

	// Swap buffers
	glfwSwapBuffers(_window->GetWindowComponent());
	glfwPollEvents();
}

DrawEngine::DrawEngine()
{
#if GL
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

#else
	
#endif
}


DrawEngine::~DrawEngine()
{
}
