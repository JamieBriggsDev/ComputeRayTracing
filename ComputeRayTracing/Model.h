#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "HardModels.h"

class Model
{
private:
	// Buffers
	GLuint m_vertexBuffer;
	GLuint m_uvBuffer;
	GLuint m_normalBuffer;
	// Element Buffer
	GLuint m_elementBuffer;

	// Buffer Data
	std::vector<glm::vec3> m_vertexBufferData;
	std::vector<glm::vec3> m_normalBufferData;
	std::vector<glm::vec2> m_uvBufferData;
	std::vector<unsigned short> m_indicesBufferData;

	// Vertex handle
	GLuint m_vertexArrayID;

	void GLBindBuffers();

public:
	// Default constructor
	Model();

	~Model();
	// Get Indices Count
	GLuint GetIndicesCount();
	// Get Vertex Buffer
	GLuint GetVertexBuffer();
	// Get UV Buffer
	GLuint GetUVBuffer();
	// Get NormalBuffer
	GLuint GetNormalBuffer();
	// Get Element buffer
	GLuint GetElementBuffer();
};

