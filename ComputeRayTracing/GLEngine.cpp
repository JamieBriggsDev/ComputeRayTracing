#if GL

#include <optional>
#include <set>

#include "GLEngine.h"
#include "GLObject.h"
#include "Controller.h"

#pragma region Debug Callbacks
void GLAPIENTRY MessageCallback(GLenum source,
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
#pragma endregion

GLEngine::GLEngine()
{
	// Create Window First before everything else
	m_myWindow = new Window();

	// Initialise Inherited Engine Code.
	Initialise();

	// Create Controller
	m_myController = new Controller();
	// Create Camera
	m_myCamera = new Camera();
	// Create Draw Engine
	m_myDrawEngine = new DrawEngine();
}

void GLEngine::Initialise()
{
	// Enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	// Create an object
	m_object = new GLObject("Resources/Models/Sphere.obj");
	// Model matrix : an identity matrix (model will be at the origin)
	m_object->SetModelMatrix(glm::mat4(1.0f));
}

#endif