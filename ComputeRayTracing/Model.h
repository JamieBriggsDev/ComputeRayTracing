#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "HardModels.h"
#include "Vertex.h"

class Model
{
protected:
	std::vector<glm::vec3> m_vertexBufferData;
	std::vector<glm::vec3> m_normalBufferData;
	std::vector<glm::vec2> m_uvBufferData;
	std::vector<uint32_t> m_indicesBufferData;

	// Buffer Data
	std::vector<Vertex> m_verticesData;


	bool LoadObj(const char* _filePath,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals);

	void NormalizePositions(std::vector<glm::vec3> &out_vertices);
	
	void indexVBO(
		std::vector<glm::vec3> & in_vertices,
		std::vector<glm::vec2> & in_uvs,
		std::vector<glm::vec3> & in_normals,

		std::vector<uint32_t> & out_indices,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals
	);

	void CombineVertexData();
public:
	// Default constructor
	Model() {};
	// Constructor which loads .obj file
	Model(const char* _objFilePath);

	~Model();

};

