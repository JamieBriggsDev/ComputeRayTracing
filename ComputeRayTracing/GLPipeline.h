#if GL

#pragma once
#include "Pipeline.h"

class GLPipeline :
	public Pipeline
{
private:
	int GLOpenVertexShader(const char* _vertexFilePath);
	int GLOpenFragmentShader(const char* _fragmentFilePath);
public:
	GLPipeline(const char* _vertexFilePath, const char* _fragmentFilePath);
	~GLPipeline();
};

#endif