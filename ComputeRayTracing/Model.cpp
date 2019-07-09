#include <objLoader.h>
#include <glm/glm.hpp>

#include <map>

//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>

#include "Model.h"
#include "Vertex.h"

Model::Model()
{
	//m_vertexBufferData = g_cube_vertex_buffer_data;
	//m_uvBufferData = g_cube_uv_buffer_data;
	m_vertexBufferData.clear();
	m_uvBufferData.clear();

	// copy arrays into vectors
	m_vertexBufferData.insert(m_vertexBufferData.end(),
		&g_cube_vertex_buffer_data[0],
		&g_cube_vertex_buffer_data[sizeof(g_cube_vertex_buffer_data)]);
	m_uvBufferData.insert(m_uvBufferData.end(),
		&g_cube_uv_buffer_data[0],
		&g_cube_uv_buffer_data[sizeof(g_cube_uv_buffer_data)]);

	BindBuffers();
}

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

	BindBuffers();
}


Model::~Model()
{
	glDeleteBuffers(1, &m_vertexBuffer);
	//glDeleteBuffers(1, &m_colorBuffer);
	glDeleteVertexArrays(1, &m_vertexArrayID);
}

void Model::BindBuffers()
{
	// Bind IDs
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	// Get buffer sizes
	int vertexBufferSize = m_vertexBufferData.size() * sizeof(glm::vec3);
	int uvBufferSize = m_uvBufferData.size() * sizeof(glm::vec2);
	int normalBufferSize = m_normalBufferData.size() * sizeof(glm::vec3);
	int indicesBufferSize = m_indicesBufferData.size() * sizeof(unsigned short);
	// Bind Vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, &m_vertexBufferData[0], GL_STATIC_DRAW);
	// Bind UV buffer
	glGenBuffers(1, &m_uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvBufferSize, &m_uvBufferData[0], GL_STATIC_DRAW);
	// Bind Normal buffer
	glGenBuffers(1, &m_normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normalBufferSize, &m_normalBufferData[0], GL_STATIC_DRAW);
	// Bind Element/ Index Buffer
	glGenBuffers(1, &m_elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesBufferSize, &m_indicesBufferData[0], GL_STATIC_DRAW);
}

// Get Indices Count
GLuint Model::GetIndicesCount()
{
	return m_indicesBufferData.size();
}

// Get Vertex Buffer
GLuint Model::GetVertexBuffer() 
{ 
	return m_vertexBuffer; 
}

// Get UV Buffer
GLuint Model::GetUVBuffer() 
{
	return m_uvBuffer;
}

// Get Normal Buffer
GLuint Model::GetNormalBuffer()
{
	return m_normalBuffer;
}

// Get Element Buffer
GLuint Model::GetElementBuffer()
{
	return m_elementBuffer;
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

	std::vector<unsigned short> temp_indices;
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
	glm::vec3 & in_vertex,
	glm::vec2 & in_uv,
	glm::vec3 & in_normal,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	unsigned short & result
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
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	std::vector<glm::vec3> & in_normals,

	std::vector<unsigned short> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	// For each input vertex
	for (unsigned int i = 0; i < in_vertices.size(); i++) {

		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex(in_vertices[i], in_uvs[i], in_normals[i], out_vertices, out_uvs, out_normals, index);

		if (found) { // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
		}
		else { // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			out_indices.push_back((unsigned short)out_vertices.size() - 1);
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
	std::map<PackedVertex, unsigned short> & VertexToOutIndex,
	unsigned short & result
) {
	std::map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
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

	std::vector<unsigned short> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	std::map<PackedVertex, unsigned short> VertexToOutIndex;

	// For each input vertex
	for (unsigned int i = 0; i < in_vertices.size(); i++) {

		PackedVertex packed = { in_vertices[i], in_uvs[i], in_normals[i] };


		// Try to find a similar vertex in out_XXXX
		unsigned short index;
		bool found = getSimilarVertexIndex_fast(packed, VertexToOutIndex, index);

		if (found) { // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
		}
		else { // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			unsigned short newindex = (unsigned short)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}
