#if GL

#pragma once
#include "Pipeline.h"

class GLPipeline :
	public Pipeline
{
private:
	// TODO clean up unneeded handles
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

	// Texture Sampler ID
	GLuint m_texSamplerID;
	// Ray Tracer Generated Texture
	GLuint m_rayTextureOutputID;
	// Create ray tracing stuff
	void GLCreateRayTexture();

	// Light Position
	GLuint m_lightPositionID;
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

	// Get ray trace texture
	GLuint GetRayTextureOutput() { return m_rayTextureOutputID; }
	// Get light position ID
	GLuint GetLightPositionID() { return m_lightPositionID; }
	~GLPipeline();
};

#endif