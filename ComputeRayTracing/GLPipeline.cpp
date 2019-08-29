#if GL
#include "GLPipeline.h"
#include "Window.h"


// GL Shader initialise.
GLPipeline::GLPipeline(const char* _vertexFilePath, const char * _fragmentFilePath,
	const char* _computeFilePath)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint ComputeShaderID = glCreateShader(GL_COMPUTE_SHADER);
	m_vertexShaderID = VertexShaderID;
	m_fragmentShaderID = FragmentShaderID;

	// Read the Vertex Shader code from the file
	GLOpenVertexShader(_vertexFilePath);

	// Read the Fragment Shader code from the file
	GLOpenFragmentShader(_fragmentFilePath);

	// Read the Compute Shader code from the file
	GLOpenComputeShader(_computeFilePath);

	// Error handling
	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", _vertexFilePath);
	char const* VertexSourcePointer = m_vertexShaderCode.c_str();
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
	char const* FragmentSourcePointer = m_fragmentShaderCode.c_str();
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

	// Compile Compute Shader
	printf("Compiling shader : %s\n", _computeFilePath);
	char const* ComputeSourcePointer = m_computeShaderCode.c_str();
	glShaderSource(ComputeShaderID, 1, &ComputeSourcePointer, NULL);
	glCompileShader(ComputeShaderID);

	// Check Compute Shader
	glGetShaderiv(ComputeShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(ComputeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ComputeShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(ComputeShaderID, InfoLogLength, NULL, &ComputeShaderErrorMessage[0]);
		printf("%s\n", &ComputeShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking vert & frag program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	printf("Linking comp program\n");
	GLuint ComputeProgramID = glCreateProgram();
	glAttachShader(ComputeProgramID, ComputeShaderID);
	glLinkProgram(ComputeProgramID);

	// Check the vert & frag program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	// Check the comp program
	glGetProgramiv(ComputeProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ComputeProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ComputeProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	glDetachShader(ComputeProgramID, ComputeShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	glDeleteShader(ComputeShaderID);

	m_programID = ProgramID;
	m_computeProgramID = ComputeProgramID;

	//glUseProgram(m_programID);
	//// Set MVP Matrix Handle
	//m_mvpMatrixID = glGetUniformLocation(m_programID, "MVP");

	// Set Texture Sampler Handle
	m_texSamplerID = glGetUniformLocation(m_programID, "TextureSampler");

	// Compute shader
	GLCreateRayTexture();
}

GLPipeline::~GLPipeline()
{
	glDeleteProgram(m_programID);
}

int GLPipeline::GLOpenVertexShader(const char* _vertexFilePath)
{
	std::ifstream VertexShaderStream(_vertexFilePath, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		m_vertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s.\n", _vertexFilePath);
		getchar();
		return -1;
	}

	return 1;
}

int GLPipeline::GLOpenFragmentShader(const char* fragment_file_path)
{
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		m_fragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s.\n", fragment_file_path);
		getchar();
		return -1;
	}

	return 1;
}

int GLPipeline::GLOpenComputeShader(const char * _computeFilePath)
{
	std::ifstream ComputeShaderStream(_computeFilePath, std::ios::in);
	if (ComputeShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << ComputeShaderStream.rdbuf();
		m_computeShaderCode = sstr.str();
		ComputeShaderStream.close();
	}
	else {
		printf("Impossible to open %s.\n", _computeFilePath);
		getchar();
		return -1;
	}

	return 1;
}

void FindWorkGroupSizes()
{
	// Get max work groups
	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n",
		work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);
	// Get max work size
	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	// Max work group invocations
	GLint work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);
}

void GLPipeline::GLCreateRayTexture()
{
	// Gen texture
	glGenTextures(1, &m_rayTextureOutputID);
	// Set as active texture
	glActiveTexture(GL_TEXTURE0); // TODO maybes TEXTURE1 if texture is used.
	// Bind texture
	glBindTexture(GL_TEXTURE_2D, m_rayTextureOutputID);
	// Texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Define resolution
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Window::s_windowWidth, Window::s_windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	// Bind image texture
	glBindImageTexture(0, m_rayTextureOutputID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Sphere Location position handle
	m_objectPositionID = glGetUniformLocation(m_computeProgramID, "SphereLocation");
	// Camera world matrix
	m_cameraWorldMatrixID = glGetUniformLocation(m_cameraWorldMatrixID, "CameraWorldMatrix");
	// Find camera position handle
	m_cameraPositionID = glGetUniformLocation(m_computeProgramID, "CameraPosition");
	// Find light position handle
	m_lightPositionID = glGetUniformLocation(m_computeProgramID, "light");
#if DEBUG
	// Check work group sizes
	FindWorkGroupSizes();
#endif

}

#endif