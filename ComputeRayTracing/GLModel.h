#if GL
#pragma once
#include "Model.h"
class GLModel :
	public Model
{
private:
	// Buffers
	GLuint m_vertexBuffer;
	GLuint m_uvBuffer;
	GLuint m_normalBuffer;
	// Element Buffer
	GLuint m_elementBuffer;

	// VAO ID
	GLuint m_vao;
	GLuint m_vbo[3];

	// Vertex handle
	GLuint m_vertexArrayID;

	void GLBindBuffers();
public:
	GLModel(const char* _objFilePath);
	~GLModel();

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

#endif