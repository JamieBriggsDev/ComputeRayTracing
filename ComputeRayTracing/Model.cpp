#include <objLoader.h>
#include <glm/glm.hpp>

#include <map>

//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>

#include "Model.h"
#include "Vertex.h"


Model::Model(const char * _objFilePath)
{
	//LoadObjOld(_filePath, m_vertexBufferData, m_uvBufferData);
	std::vector<glm::vec3> vertexBufferData;
	std::vector<glm::vec3> normalBufferData;
	std::vector<glm::vec2> uvBufferData;
	LoadObj(_objFilePath, vertexBufferData, uvBufferData, normalBufferData);
	m_indicesBufferData.clear();
	indexVBO(vertexBufferData, uvBufferData, normalBufferData,
		m_indicesBufferData, m_vertexBufferData, m_uvBufferData, m_normalBufferData);
	// Combine vertex data
	CombineVertexData();
}


Model::~Model()
{

}

bool Model::LoadObj(const char * _filePath, 
	std::vector<glm::vec3>& out_vertices, 
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals)
{
	// Clear vertices and UVS
	out_vertices.clear();
	out_uvs.clear();
	out_normals.clear();

	printf("Loading OBJ file %s...\n", _filePath);

	std::vector<uint32_t> temp_indices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	// Initialise obj loader
	objl::Loader MyLoader;
	
	if (!MyLoader.LoadFile(_filePath) ){
		printf("Can't Open obj. file!\n");
		return false;
	}

	for (const auto& mesh : MyLoader.LoadedMeshes)
	{
		for (const auto& vertex : mesh.Vertices)
		{
			glm::vec3 position;
			position.x = vertex.Position.X;
			position.y = vertex.Position.Y;
			position.z = vertex.Position.Z;
			temp_vertices.push_back(position);

			glm::vec2 uv;
			uv.x = vertex.TextureCoordinate.X;
			uv.y = vertex.TextureCoordinate.Y;
			temp_uvs.push_back(uv);

			glm::vec3 normal;
			normal.x = vertex.Normal.X;
			normal.y = vertex.Normal.Y;
			normal.z = vertex.Normal.Z;
			
		}

		for (const auto& index : mesh.Indices)
		{
			temp_indices.push_back(index);
		}
	}

	m_indicesBufferData = temp_indices;

	for (const auto& index : temp_indices)
	{
		// Add position data
		glm::vec3 position;
		position.x = MyLoader.LoadedVertices[index].Position.X;
		position.y = MyLoader.LoadedVertices[index].Position.Y;
		position.z = MyLoader.LoadedVertices[index].Position.Z;
		out_vertices.push_back(position);
		// Add uv data
		glm::vec2 uv;
		uv.x = MyLoader.LoadedVertices[index].TextureCoordinate.X;
		uv.y = MyLoader.LoadedVertices[index].TextureCoordinate.Y;
		out_uvs.push_back(uv);
		// add normal data
		glm::vec3 normal;
		normal.x = MyLoader.LoadedVertices[index].Normal.X;
		normal.y = MyLoader.LoadedVertices[index].Normal.Y;
		normal.z = MyLoader.LoadedVertices[index].Normal.Z;
		out_normals.push_back(normal);
	}

	// Normalize Positions
	NormalizePositions(out_vertices);

	return true;
}

void Model::NormalizePositions(std::vector<glm::vec3> &out_vertices)
{
	float l_largest = 0.0f;
	for (int i = 0; i < out_vertices.size(); i++)
	{
		float t_length = sqrt((out_vertices[i].x * out_vertices[i].x) +
			(out_vertices[i].y * out_vertices[i].y) +
			(out_vertices[i].z * out_vertices[i].z));

		if (t_length > l_largest) 
			l_largest = t_length;
	}//for

	//printf("Normalizing model size by factor %f\n", l_largest);
	for (int i = 0; i < out_vertices.size(); i++)
	{
		out_vertices[i].x = out_vertices[i].x / l_largest;
		out_vertices[i].y = out_vertices[i].y / l_largest;
		out_vertices[i].z = out_vertices[i].z / l_largest;
	}//for
}

// Returns true iif v1 can be considered equal to v2
bool is_near(float v1, float v2) {
	return fabs(v1 - v2) < 0.01f;
}

// Searches through all already-exported vertices
// for a similar one.
// Similar = same position + same UVs + same normal
bool getSimilarVertexIndex(
	glm::vec3 &in_vertex,
	glm::vec2 &in_uv,
	glm::vec3 &in_normal,
	std::vector<glm::vec3> &out_vertices,
	std::vector<glm::vec2> &out_uvs,
	std::vector<glm::vec3> &out_normals,
	uint32_t &result
) {
	// Lame linear search
	for (unsigned int i = 0; i < out_vertices.size(); i++) {
		if (
			is_near(in_vertex.x, out_vertices[i].x) &&
			is_near(in_vertex.y, out_vertices[i].y) &&
			is_near(in_vertex.z, out_vertices[i].z) &&
			is_near(in_uv.x, out_uvs[i].x) &&
			is_near(in_uv.y, out_uvs[i].y) &&
			is_near(in_normal.x, out_normals[i].x) &&
			is_near(in_normal.y, out_normals[i].y) &&
			is_near(in_normal.z, out_normals[i].z)
			) {
			result = i;
			return true;
		}
	}
	// No other vertex could be used instead.
	// Looks like we'll have to add it to the VBO.
	return false;
}

void indexVBO_slow(
	std::vector<glm::vec3> &in_vertices,
	std::vector<glm::vec2> &in_uvs,
	std::vector<glm::vec3> &in_normals,

	std::vector<uint32_t> &out_indices,
	std::vector<glm::vec3> &out_vertices,
	std::vector<glm::vec2> &out_uvs,
	std::vector<glm::vec3> &out_normals
) {
	// For each input vertex
	for (unsigned int i = 0; i < in_vertices.size(); i++) {

		// Try to find a similar vertex in out_XXXX
		uint32_t index;
		bool found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i], out_vertices, out_uvs, out_normals, index);

		if (found) { // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
		}
		else { // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			out_indices.push_back((uint32_t)out_vertices.size() - 1);
		}
	}
}

struct PackedVertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const {
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
	};
};

bool getSimilarVertexIndex_fast(
	PackedVertex & packed,
	std::map<PackedVertex, uint32_t> & VertexToOutIndex,
	uint32_t & result
) {
	std::map<PackedVertex, uint32_t>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end()) {
		return false;
	}
	else {
		result = it->second;
		return true;
	}
}

void Model::indexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	std::vector<glm::vec3> & in_normals,

	std::vector<uint32_t> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	std::map<PackedVertex, uint32_t> VertexToOutIndex;

	// For each input vertex
	for (unsigned int i = 0; i < in_vertices.size(); i++) {

		PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };


		// Try to find a similar vertex in out_XXXX
		uint32_t index;
		bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);

		if (found) { // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
		}
		else { // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			uint32_t newindex = (uint32_t)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}

void Model::CombineVertexData()
{
	for(int i = 0; i < m_vertexBufferData.size(); i++)
	{
		Vertex newVertex;
		// Position Data
		newVertex.pos.x = m_vertexBufferData[i].x;
		newVertex.pos.y = m_vertexBufferData[i].y;
		newVertex.pos.z = m_vertexBufferData[i].z;
		// Normal Data
		newVertex.normal.x = m_normalBufferData[i].x;
		newVertex.normal.y = m_normalBufferData[i].y;
		newVertex.normal.z = m_normalBufferData[i].z;
		// UV Data
		newVertex.texCoord.x = m_uvBufferData[i].x;
		newVertex.texCoord.y = m_uvBufferData[i].y;
		// Add to vertices
		m_verticesData.push_back(newVertex);
	}
	std::cout << "Total Pos\t: " << m_vertexBufferData.size() << std::endl;
	std::cout << "Total Norm\t: " << m_normalBufferData.size() << std::endl;
	std::cout << "Total UVData\t: " << m_uvBufferData.size() << std::endl;

	std::cout << "Total Vertices: " << m_vertexBufferData.size() << std::endl;
}
