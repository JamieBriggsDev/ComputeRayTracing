#include "Shader.h"

std::vector<char> Shader::ReadFile(const std::string& filename)
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

Shader::~Shader()
{
	
}







