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

class Pipeline
{
protected:
	
	std::string m_computeShaderCode;
	std::string m_vertexShaderCode;
	std::string m_fragmentShaderCode;

	std::vector<char> ReadFile(const std::string& filename);
public:
	Pipeline() {};
	~Pipeline();

};

