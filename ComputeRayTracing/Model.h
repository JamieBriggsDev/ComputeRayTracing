#pragma once

#include <GL/glew.h>
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

	void BindBuffers();

	bool LoadObj(const char* _filePath,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals);

	void NormalizePositions(std::vector<glm::vec3> &out_vertices);
	
	void indexVBO(
		std::vector<glm::vec3> & in_vertices,
		std::vector<glm::vec2> & in_uvs,
		std::vector<glm::vec3> & in_normals,

		std::vector<unsigned short> & out_indices,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals
	);
public:
	// Default constructor
	Model();
	// Constructor which loads .obj file
	Model(const char* _objFilePath);

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

