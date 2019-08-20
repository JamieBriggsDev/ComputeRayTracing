#if GL

#pragma once
#include "Pipeline.h"

class GLPipeline :
	public Pipeline
{
private:
	int GLOpenVertexShader(const char* _vertexFilePath);
	int GLOpenFragmentShader(const char* _fragmentFilePath);
	int GLOpenComputeShader(const char* _computeFilePath);
	// Vertex shader ID
	GLuint m_vertexShaderID;
	// Fragment shader ID
	GLuint m_fragmentShaderID;
	// Compute shader ID
	GLuint m_computeShaderID;
	// Program ID
	GLuint m_programID;
	// Compute Program ID
	GLuint m_computeProgramID;
	// Matrix ID
	GLuint m_mvpMatrixID;
	// View Matrix ID // TODO Remove
	GLuint m_viewMatrixID;
	// Model Matrix ID
	GLuint m_modelMatrixID;
	// Light Position World Space
	GLuint m_lightPositionWorldSpace;
	// Texture Sampler ID
	GLuint m_texSamplerID;

	// Compute Shader Stuff
	GLuint m_rayTextureOutput;
	void GLCreateRayTexture();
public:
	GLPipeline(const char* _vertexFilePath, const char* _fragmentFilePath,
		const char* _computeFilePath);
	// Get shader program ID
	GLuint GetProgramID() { return m_programID; }
	// Get compute program ID
	GLuint GetComputeProgramID() { return m_computeProgramID; }
	// Get vertex shader ID
	GLuint GetVertexShaderID() { return m_vertexShaderID; }
	// Get fragment shader ID
	GLuint GetFragmentShaderID() { return m_fragmentShaderID; }
	// Get compute shader ID
	GLuint GetComputeShaderID() { return m_computeShaderID; }
	// Get texture sampler ID
	GLuint GetTextureSamplerID() { return m_texSamplerID; }
	// Get view matrix ID
	GLuint GetViewMatrixID() { return m_viewMatrixID; }
	// Get model matrix ID
	GLuint GetModelMatrixID() { return m_modelMatrixID; }
	// Get MVP ID
	GLuint GetMVPID() { return m_mvpMatrixID; }
	//int GetModelViewMatrixID() { return m_modelViewMatrixID; }
	GLuint GetLightPositionWorldSpaceID() { return m_lightPositionWorldSpace; }
	~GLPipeline();
};

#endif