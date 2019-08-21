#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Model.h"
#include "Texture.h"

class Camera;

class Object
{
protected:
	// Model
	Model* m_model;
	// Model matrix
	glm::mat4 m_modelMatrix;
	// World position 
	glm::vec3 m_position;
	// Texture
	Texture* m_texture;
	// Texture
	Texture* m_heightMap;


public:
	Object() {};
	~Object();
	// Get Model
	Model* GetModel() { return m_model; }
	// Get Model Matrix
	glm::mat4 GetModelMatrix() { return m_modelMatrix; }
	// Get Texture TODO REMOVE?
	Texture* GetTexture() { return m_texture; }
	// Get position
	glm::vec3 GetPosition() { return m_position; }

	// Set Model Matrix
	void SetModelMatrix(glm::mat4 _modelMatrix);

	virtual void Update(float _deltaTime) = 0;
};

