#include "Window.h"

#include "Controller.h"

#include <vulkan/vulkan.h>

// Error codes
#define W_GLFW_FAILED_TO_INITIALISE -100
#define W_WINDOW_FAILED_TO_OPEN -101
#define W_COULD_NOT_INITIALISE_GLEW -102

int Window::s_windowWidth = WINDOW_WIDTH;
int Window::s_windowHeight = WINDOW_HEIGHT;

#if GL
void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	const char* t_severity;
	if (severity == GL_DEBUG_SEVERITY_HIGH)
		t_severity = "HIGH";
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
		t_severity = "MEDIUM";
	else if (severity == GL_DEBUG_SEVERITY_LOW)
		t_severity = "LOW";
	else
		t_severity = "";

	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = %s, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, t_severity, message);
}

#elif VK
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

#endif




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

	// Initialize GLEW
	glfwMakeContextCurrent(m_window); 
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return W_COULD_NOT_INITIALISE_GLEW;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

	
#if GL
	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	// Enable V-Sync
	glfwSwapInterval(0); // v sync
#endif


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
