#if GL

#include "GLObject.h"
#include "GLPipeline.h"
#include "Camera.h"
#include "UniformBufferObject.h"

GLObject::GLObject(const char* _modelFilePath)
{
	// Load Model
	m_model = new GLModel(_modelFilePath);
	//m_pipeline = new GLPipeline("Shaders/Shader.vert",
	//	"Shaders/Shader.frag");
	// Set model Matrix
	m_modelMatrix = glm::mat4();
	// Set world position
	m_position = glm::vec3(0.0f, 0.0f, -5.0f);
}

GLObject::~GLObject()
{
}

void GLObject::Update(float _deltaTime)
{
	static float total = 0.0f;
	total += _deltaTime;
	m_position.x = glm::sin(total) * 2;
	m_position.y = glm::cos(total) * 2;
	// Init timer
	static float Timer;
	if (Timer == NULL)
		Timer = _deltaTime;
	else
		Timer += _deltaTime;

	m_modelMatrix = glm::rotate(glm::mat4(1.0f), Timer * glm::radians(90.0f), m_position);
}


#endif