#include "Window.h"

#include "Controller.h"

#if VK
#include <vulkan/vulkan.h>
#endif

// Error codes
#define W_GLFW_FAILED_TO_INITIALISE -100
#define W_WINDOW_FAILED_TO_OPEN -101
#define W_COULD_NOT_INITIALISE_GLEW -102

int Window::s_windowWidth = WINDOW_WIDTH;
int Window::s_windowHeight = WINDOW_HEIGHT;


Window::Window()
{
	Initialise();
}

Window::~Window()
{
	// Clean up
	// Delete objects
	//delete m_cube;
}

int Window::Initialise()
{	
	// Initialise GLFW
	glewExperimental = true; // Needed for core profile
	int err = glfwInit();
	// Try to initialise
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return W_GLFW_FAILED_TO_INITIALISE;
	}

#if GL

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
#elif VK
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
	// Ensure window is not resizable
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	// Open a window and create its OpenGL context
	m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracing Comparison", NULL, NULL);
	if (m_window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. \n");
		glfwTerminate();
		return W_WINDOW_FAILED_TO_OPEN;
	}
#if GL
	// Initialize GLEW
	glfwMakeContextCurrent(m_window); 
	glewExperimental = true; // Needed in core profile
	err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return W_COULD_NOT_INITIALISE_GLEW;
	}
#endif
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);


	// Reset mouse position to middle of window
	glfwSetCursorPos(m_window,
		Window::s_windowWidth / 2,
		Window::s_windowHeight / 2);

	
	return 1;
}

void Window::Update()
{

}

GLFWwindow* Window::GetWindowComponent()
{
	return m_window;
} 

int Window::CheckWindowClose(Controller* _controller)
{
	// returns false if ESC not pressed or window isn't attempting to 
	//  close.
	int ans = _controller->IsKeyPressed(this, GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(m_window) == 0;

	return ans;
}
