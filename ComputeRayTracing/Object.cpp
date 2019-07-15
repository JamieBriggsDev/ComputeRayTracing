#include "Object.h"
#include "Camera.h"

//Object::Object(const char * _modelFilePath, TextureType _textureType, const char * _textureFilePath)
//{
//	m_model = new Model(_modelFilePath);
//	m_shader = new Shader("Shaders/TexturedVert.vert",
//		"Shaders/TexturedFrag.frag");
//	m_modelMatrix = glm::mat4();
//	m_texture = new Texture(_textureType, _textureFilePath);
//}
//
//Object::Object(const char* _modelFilePath, TextureType _textureType,
//	const char* _textureFilePath, const char* _heightMapFilePath)
//{
//	m_model = new Model(_modelFilePath);
//	m_shader = new Shader("Shaders/TexturedHeightVert.vert",
//		"Shaders/TexturedHeightFrag.frag");
//	m_modelMatrix = glm::mat4();
//	m_texture = new Texture(_textureType, _textureFilePath);
//	m_heightMap = new Texture(_textureType, _heightMapFilePath);
//}


IObject::~IObject()
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
void IObject::SetModelMatrix(glm::mat4 _modelMatrix) 
{ 
	m_modelMatrix = _modelMatrix; 
}
