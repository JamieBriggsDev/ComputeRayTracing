#include "Object.h"

Object::~Object()
{
	// Clean up model
	delete m_model;
	// Clean up shader
	delete m_pipeline;
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
