#include "Object.h"
#include "Camera.h"

Object::Object()
{
	m_model = new Model();
	m_shader = new Shader("Shaders/SimpleVert.vert", 
		"Shaders/SimpleFrag.frag");
	m_modelMatrix = glm::mat4();
	
}

Object::Object(const char * _modelFilePath)
{
	m_model = new Model(_modelFilePath);
	m_shader = new Shader("Shaders/SimpleVert.vert",
		"Shaders/SimpleFrag.frag");
	m_modelMatrix = glm::mat4();
}

Object::Object(const char * _modelFilePath, TextureType _textureType, const char * _textureFilePath)
{
	m_model = new Model(_modelFilePath);
	m_shader = new Shader("Shaders/TexturedVert.vert",
		"Shaders/TexturedFrag.frag");
	m_modelMatrix = glm::mat4();
	m_texture = new Texture(_textureType, _textureFilePath);
}
Object::Object(const char* _modelFilePath, TextureType _textureType,
	const char* _textureFilePath, const char* _heightMapFilePath)
{
	m_model = new Model(_modelFilePath);
	m_shader = new Shader("Shaders/TexturedHeightVert.vert",
		"Shaders/TexturedHeightFrag.frag");
	m_modelMatrix = glm::mat4();
	m_texture = new Texture(_textureType, _textureFilePath);
	m_heightMap = new Texture(_textureType, _heightMapFilePath);
}


Object::~Object()
{
	// Clean up model
	delete m_model;
	// Clean up shader
	delete m_shader;
	// Clean up texture
	delete m_texture;
	// Clean up height map texture
	delete m_heightMap;
}

// Set model matrix
void Object::SetModelMatrix(glm::mat4 _modelMatrix) 
{ 
	m_modelMatrix = _modelMatrix; 
}

// Draw function
void Object::GLDraw(Camera* _camera)
{
	// MVP
	glm::mat4 MVP = _camera->GetProjectionView() * m_modelMatrix;
	glm::mat4 M = m_modelMatrix;
	glm::mat4 V = _camera->GetView();
	glm::mat4 MV = M * V;

	// Send our transformations to the shader
	glUniformMatrix4fv(m_shader->GetMVPID(), 1, GL_FALSE, &MVP[0][0]);
	//std::cout << "MVP: " << glGetError() << std::endl; 
	glUniformMatrix4fv(m_shader->GetModelMatrixID(), 1, GL_FALSE, &M[0][0]);
	//std::cout << "M: " << glGetError() << std::endl;
	glUniformMatrix4fv(m_shader->GetViewMatrixID(), 1, GL_FALSE, &V[0][0]);
	//std::cout << "V: " << glGetError() << std::endl;
	
	// Send light position to the shader
	glm::vec3 lightPosition = glm::vec3(4, 4, 4);
	glUniform3f(m_shader->GetLightPositionWorldSpaceID(), 
		lightPosition.x, lightPosition.y, lightPosition.z);
	//std::cout << "Light: " << glGetError() << std::endl;

	if (m_texture)
	{
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture->GetData());
		// Set TextureSampler sampler to use Texture Unit 0
		glUniform1i(m_shader->GetTextureSamplerID(), 0);
		//std::cout << "Texture Sampler: " << glGetError() << std::endl;
	}

	if (m_heightMap)
	{
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_heightMap->GetData());
		// Set TextureSampler sampler to use Texture Unit 0
		glUniform1i(m_shader->GetHeightMapSamplerID(), 1);
		//std::cout << "Height Map Sampler: " << glGetError() << std::endl;
	}
	
	// 1rst attribute buffer : vertices
  	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_model->GetVertexBuffer());
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);


		// 2nd attribute buffer : UV
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_model->GetUVBuffer());
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
	

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_model->GetNormalBuffer());
	glVertexAttribPointer(
		2,                  // attribute 2. must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_model->GetElementBuffer());

	// Draw the triangle !
 	//glDrawArrays(GL_TRIANGLES, 0, m_model->GetIndicesCount());
	glDrawElements(
		GL_TRIANGLES,
		m_model->GetIndicesCount(),
		GL_UNSIGNED_SHORT,
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

// Draw function
void Object::VKDraw(Camera* _camera)
{
	// TODO : VULKAN DRAW CALL.
}