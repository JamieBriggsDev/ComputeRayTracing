#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

class Shader
{
private:
	// Program ID
	GLuint m_programID;
	// Matrix ID
	GLuint m_mvpMatrixID;
	// View Matrix ID
	GLuint m_viewMatrixID;
	// Model Matrix ID
	GLuint m_modelMatrixID;
	// Light Position World Space
	GLuint m_lightPositionWorldSpace;
	// Texture Sampler ID
	GLuint m_texSamplerID;
	// Height Map Sampler ID
	GLuint m_heightSamplerID;

	std::string vertexShaderCode;
	std::string fragmentShaderCode;

	int OpenVertexShader(const char* vertex_file_path);
	int OpenFragmentShader(const char* fragment_file_path);
public:
	Shader(const char* vertex_file_path, const char * fragment_file_path);
	~Shader();
	GLuint GetProgramID(){ return m_programID; }
	GLuint GetTextureSamplerID() { return m_texSamplerID; }
	GLuint GetHeightMapSamplerID() { return m_heightSamplerID; }
	GLuint GetViewMatrixID() { return m_viewMatrixID; }
	GLuint GetModelMatrixID() { return m_modelMatrixID; }
	GLuint GetMVPID() { return m_mvpMatrixID; }
	//int GetModelViewMatrixID() { return m_modelViewMatrixID; }
	GLuint GetLightPositionWorldSpaceID() { return m_lightPositionWorldSpace; }
};

