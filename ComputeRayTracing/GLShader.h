#if GL

#pragma once
#include "Shader.h"
class GLShader :
	public Shader
{
private:
	int GLOpenVertexShader(const char* _vertexFilePath);
	int GLOpenFragmentShader(const char* _fragmentFilePath);
public:
	GLShader(const char* _vertexFilePath, const char * _fragmentFilePath);
	~GLShader();
};

#endif