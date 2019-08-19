#if GL
#include "GLModel.h"

GLModel::GLModel(const char* _objFilePath) : Model(_objFilePath)
{
	// Bind buffers
	GLBindBuffers();
}


GLModel::~GLModel()
{
	// Clean up GL stuff
	glDeleteBuffers(1, &m_vertexBuffer);
	//glDeleteBuffers(1, &m_colorBuffer);
	glDeleteVertexArrays(1, &m_vertexArrayID);
}

void GLModel::GLBindBuffers()
{
	// Bind IDs
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	// Get buffer sizes
	int vertexBufferSize = m_verticesData.size() * sizeof(glm::vec3);
	int normalBufferSize = m_verticesData.size() * sizeof(glm::vec3);
	int uvBufferSize = m_verticesData.size() * sizeof(glm::vec2);
	int indicesBufferSize = m_indicesBufferData.size() * sizeof(uint32_t);
	// Bind Vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, &m_vertexBufferData[0], GL_STATIC_DRAW);
	// Bind Normal buffer
	glGenBuffers(1, &m_normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normalBufferSize, &m_normalBufferData[0], GL_STATIC_DRAW);
	// Bind UV bufferw
	glGenBuffers(1, &m_uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvBufferSize, &m_uvBufferData[0], GL_STATIC_DRAW);
	// Bind Element/ Index Buffer
	glGenBuffers(1, &m_elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesBufferSize, &m_indicesBufferData[0], GL_STATIC_DRAW);
}

// Get Indices Count
GLuint GLModel::GetIndicesCount()
{
	return m_indicesBufferData.size();
}

// Get Vertex Buffer
GLuint GLModel::GetVertexBuffer()
{
	return m_vertexBuffer;
}

// Get UV Buffer
GLuint GLModel::GetUVBuffer()
{
	return m_uvBuffer;
}

// Get Normal Buffer
GLuint GLModel::GetNormalBuffer()
{
	return m_normalBuffer;
}

// Get Element Buffer
GLuint GLModel::GetElementBuffer()
{
	return m_elementBuffer;
}

#endif