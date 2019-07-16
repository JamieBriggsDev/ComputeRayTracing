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
protected:
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

	std::vector<char> ReadFile(const std::string& filename);
public:
	Shader() {};
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

