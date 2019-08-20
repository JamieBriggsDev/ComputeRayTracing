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
		"Shaders/GLShader.frag");
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

void GLObject::Draw(Camera* _camera, float _deltaTime)
{
	// MVP
	glm::mat4 MVP = _camera->GetProjectionView() * m_modelMatrix;
	glm::mat4 M = m_modelMatrix;
	glm::mat4 V = _camera->GetView();
	glm::mat4 MV = M * V;

	// Send our transformations to the shader
	glUniformMatrix4fv(m_pipeline->GetMVPID(), 1, GL_FALSE, &MVP[0][0]);
	//std::cout << "MVP: " << glGetError() << std::endl; 
	//glUniformMatrix4fv(m_pipeline->GetModelMatrixID(), 1, GL_FALSE, &M[0][0]);
	////std::cout << "M: " << glGetError() << std::endl;
	//glUniformMatrix4fv(m_pipeline->GetViewMatrixID(), 1, GL_FALSE, &V[0][0]);
	//std::cout << "V: " << glGetError() << std::endl;

	// Send light position to the shader
	//glm::vec3 lightPosition = glm::vec3(4, 4, 4);
	//glUniform3f(m_pipeline->GetLightPositionWorldSpaceID(),
	//	lightPosition.x, lightPosition.y, lightPosition.z);


	if (m_texture)
	{
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture->GetData());
		// Set TextureSampler sampler to use Texture Unit 0
		glUniform1i(m_pipeline->GetTextureSamplerID(), 0);
		//std::cout << "Texture Sampler: " << glGetError() << std::endl;
	}

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLModel*>(m_model)->GetVertexBuffer());
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLModel*>(m_model)->GetNormalBuffer());
	glVertexAttribPointer(
		1,                  // attribute 2. must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UV
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLModel*>(m_model)->GetUVBuffer());
	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);



	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLModel*>(m_model)->GetElementBuffer());

	// Draw the triangle !
	//glDrawArrays(GL_TRIANGLES, 0, m_model->GetIndicesCount());
	glDrawElements(
		GL_TRIANGLES,
		static_cast<GLModel*>(m_model)->GetIndicesCount(),
		GL_UNSIGNED_INT,
		(void*)0
	);

	// Check for any errors
	//GLenum err = glGetError();
	//int count = 0;
	//while (err != GL_NO_ERROR)
	//{
	//	std::cout << count++ << ": 0x" << std::hex << err << std::endl;
	//	err = glGetError();
	//}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

#endif