#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Pipeline.h"
#include "Model.h"
#include "Texture.h"

class Camera;

class Object
{
protected:
	// Model
	Model* m_model;
	// Shader
	Pipeline* m_pipeline;
	// Model matrix
	glm::mat4 m_modelMatrix;
	// Texture
	Texture* m_texture;
	// Texture
	Texture* m_heightMap;


public:
	//Object(VkDevice _vkDevice);

	Object() {};

	//Object(const char* _modelFilePath, TextureType _textureType, 
	//	const char* _textureFilePath);
	//Object(const char* _modelFilePath, TextureType _textureType,
	//	const char* _textureFilePath, const char* _heightMapFilePath);

	~Object();
	// Get functions
	Model* GetModel() { return m_model; }
	Pipeline* GetPipeline() { return m_pipeline; }
	glm::mat4 GetModelMatrix() { return m_modelMatrix; }
	// Set function
	void SetModelMatrix(glm::mat4 _modelMatrix);

	virtual void Update(float _deltaTime) = 0;

	virtual void Draw(Camera* _camera, float _deltaTime) = 0;
};

