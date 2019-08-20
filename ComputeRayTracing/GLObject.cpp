#if GL

#include "GLObject.h"
#include "GLPipeline.h"
#include "Camera.h"
#include "UniformBufferObject.h"

GLObject::GLObject(const char* _modelFilePath)
{
	// Load Model
	m_model = new GLModel(_modelFilePath);
	// Load Shader
	m_pipeline = new GLPipeline("Shaders/GLShader.vert",
		"Shaders/GLShader.frag",
		"Shaders/GLShader.comp");
	//m_pipeline = new GLPipeline("Shaders/Shader.vert",
	//	"Shaders/Shader.frag");
	// Set model Matrix
	m_modelMatrix = glm::mat4();
}

GLObject::~GLObject()
{
}

void GLObject::Update(float _deltaTime)
{
	// Init timer
	static float Timer;
	if (Timer == NULL)
		Timer = _deltaTime;
	else
		Timer += _deltaTime;

	m_modelMatrix = glm::rotate(glm::mat4(1.0f), Timer * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}


#endif