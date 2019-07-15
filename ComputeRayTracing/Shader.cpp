#include "Shader.h"


#define COULD_NOT_LOAD_VERTEX_SHADER 200;
#define COULD_NOT_LOAD_FRAGMENT_SHADER 201;

#if VK
static std::vector<char> ReadFile(const std::string& filename) 
{
	// Create ifstream.
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	// Check if file successfully opened.
	if (!file.is_open()) 
	{
		throw std::runtime_error("Failed to open file!");
	}
	// Get file size and create vector of file size.
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	// Go to start of file.
	file.seekg(0);
	// Read file and place into buffer.
	file.read(buffer.data(), fileSize);
	// Close the file.
	file.close();
	// Return file.
	return buffer;
}

VkShaderModule CreateShaderModule(VkDevice _vkDevice, const std::vector<char>& _code) 
{
	// Shader module create info.
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = _code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());
	// Create shader module.
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(_vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to create shader module!");
	}
	// Return the shader module.
	return shaderModule;
}
#endif

Shader::Shader(VkDevice _vkDevice, const char* _vertexFilePath, const char* _fragmentFilePath)
{
#if GL
	GLInit(_vertexFilePath, _fragmentFilePath);
#elif VK
	VKInit(_vkDevice, _vertexFilePath, _fragmentFilePath);
#endif
}

Shader::~Shader()
{
	glDeleteProgram(m_programID);
}

void Shader::GLInit(const char * _vertexFilePath, const char * _fragmentFilePath)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	GLOpenVertexShader(_vertexFilePath);

	// Read the Fragment Shader code from the file
	GLOpenFragmentShader(_fragmentFilePath);

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", _vertexFilePath);
	char const * VertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", _fragmentFilePath);
	char const * FragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	m_programID = ProgramID;
	glUseProgram(m_programID);
	// Set MVP Matrix Handle
	m_mvpMatrixID = glGetUniformLocation(m_programID, "MVP");
	// Set View Matrix Handle
	m_viewMatrixID = glGetUniformLocation(m_programID, "V");
	// Set Model Matrix Handle
	m_modelMatrixID = glGetUniformLocation(m_programID, "M");
	// Set MV Matrix Handle
	//m_modelViewMatrixID = glGetUniformLocation(m_programID, "MV");
	// Set MVP Matrix Handle
	m_lightPositionWorldSpace = glGetUniformLocation(m_programID, "LightPosition_worldspace");
	// Set Texture Sampler Handle
	m_texSamplerID = glGetUniformLocation(m_programID, "TextureSampler");
	// Set Height Map Sampler Handle
	m_heightSamplerID = glGetUniformLocation(m_programID, "HeightMapSampler");
}


int Shader::GLOpenVertexShader(const char* _vertexFilePath)
{
	std::ifstream VertexShaderStream(_vertexFilePath, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		vertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s.\n", _vertexFilePath);
		getchar();
		return COULD_NOT_LOAD_VERTEX_SHADER;
	}

	return 1;
}

int Shader::GLOpenFragmentShader(const char* fragment_file_path)
{
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		fragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s.\n", fragment_file_path);
		getchar();
		return COULD_NOT_LOAD_FRAGMENT_SHADER;
	}

	return 1;
}

void Shader::VKInit(VkDevice _vkDevice, const char* _vertexFilePath, const char* _fragmentFilePath)
{
	// Get the shader code.
	std::vector<char> fragShaderCode = ReadFile(_fragmentFilePath);
	std::vector<char> vertShaderCode = ReadFile(_vertexFilePath);

	VkShaderModule fragShaderModule = CreateShaderModule(_vkDevice, fragShaderCode);
	VkShaderModule vertShaderModule = CreateShaderModule(_vkDevice, vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	vkDestroyShaderModule(_vkDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(_vkDevice, vertShaderModule, nullptr);
}

