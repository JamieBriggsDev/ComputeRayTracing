#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Model.h"
#include "Shader.h"
#include "Texture.h"

class Camera;

class IObject
{
protected:
	// Model
	Model* m_model;
	// Shader
	Shader* m_shader;
	// Model matrix
	glm::mat4 m_modelMatrix;
	// Texture
	Texture* m_texture;
	// Texture
	Texture* m_heightMap;


public:
	//Object(VkDevice _vkDevice);

	IObject() {};

	//Object(const char* _modelFilePath, TextureType _textureType, 
	//	const char* _textureFilePath);
	//Object(const char* _modelFilePath, TextureType _textureType,
	//	const char* _textureFilePath, const char* _heightMapFilePath);

	~IObject();
	// Get functions
	Model* GetModel() { return m_model; }
	Shader* GetShader() { return m_shader; }
	glm::mat4 GetModelMatrix() { return m_modelMatrix; }
	// Set function
	void SetModelMatrix(glm::mat4 _modelMatrix);

	virtual void Draw(Camera* _camera) = 0;
#if GL
	// Draw function OpenGL
	void GLDraw(Camera* _camera);
#elif VK
	// Draw function Vulkan
	void VKDraw(Camera* _camera);
#endif
};

