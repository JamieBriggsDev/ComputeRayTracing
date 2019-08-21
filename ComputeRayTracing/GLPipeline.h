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

	// Camera Position
	GLuint m_cameraPosition;
	// Matrix ID
	GLuint m_mvpMatrixID;


	// Texture Sampler ID
	GLuint m_texSamplerID;
	// Ray Tracer Generated Texture
	GLuint m_rayTextureOutputID;
	// Camera world matrix
	GLuint m_cameraWorldMatrixID;
	// Create ray tracing stuff
	void GLCreateRayTexture();

	// View Matrix ID // TODO Remove
	GLuint m_viewMatrixID;
	// Model Matrix ID
	GLuint m_modelMatrixID;
	// Light Position World Space
	GLuint m_lightPositionWorldSpaceID;
	// Object Position
	GLuint m_objectPositionID;
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
	// Get view matrix ID
	GLuint GetViewMatrixID() { return m_viewMatrixID; }
	// Get model matrix ID
	GLuint GetModelMatrixID() { return m_modelMatrixID; }
	// Get MVP ID
	GLuint GetMVPID() { return m_mvpMatrixID; }
	//int GetModelViewMatrixID() { return m_modelViewMatrixID; }

	// Get texture sampler ID
	GLuint GetTextureSamplerID() { return m_texSamplerID; }
	// Get camera world matrix
	GLuint GetCameraWorldMatrixID() { return m_cameraWorldMatrixID; }
	// Get light position world space
	GLuint GetLightPositionWorldSpaceID() { return m_lightPositionWorldSpaceID; }
	// Get object position ID
	GLuint GetObjectPositionID() { return m_objectPositionID; }
	// Get ray trace texture
	GLuint GetRayTextureOutput() { return m_rayTextureOutputID; }
	~GLPipeline();
};

#endif