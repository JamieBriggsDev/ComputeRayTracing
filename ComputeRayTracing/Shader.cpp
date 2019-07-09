#include "Shader.h"

#define COULD_NOT_LOAD_VERTEX_SHADER 200;
#define COULD_NOT_LOAD_FRAGMENT_SHADER 201;

Shader::Shader(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	OpenVertexShader(vertex_file_path);

	// Read the Fragment Shader code from the file
	OpenFragmentShader(fragment_file_path);

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
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
	printf("Compiling shader : %s\n", fragment_file_path);
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

Shader::~Shader()
{
	glDeleteProgram(m_programID);
}

int Shader::OpenVertexShader(const char* vertex_file_path)
{
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		vertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s.\n", vertex_file_path);
		getchar();
		return COULD_NOT_LOAD_VERTEX_SHADER;
	}

	return 1;
}

int Shader::OpenFragmentShader(const char* fragment_file_path)
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


