#if GL

#pragma once
#include "Pipeline.h"

class GLPipeline :
	public Pipeline
{
private:
	int GLOpenVertexShader(const char* _vertexFilePath);
	int GLOpenFragmentShader(const char* _fragmentFilePath);
	GLuint m_vertexShaderID;
	GLuint m_fragmentShaderID;
public:
	GLPipeline(const char* _vertexFilePath, const char* _fragmentFilePath);
	GLuint GetVertexShaderID() { return m_vertexShaderID; }
	GLuint GetFragmentShaderID() { return m_fragmentShaderID; }
	~GLPipeline();
};

#endif