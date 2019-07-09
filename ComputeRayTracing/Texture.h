#pragma once

#include <GL/glew.h>

enum class TextureType {BMP};

class Texture
{
private:
	GLuint m_data;
	TextureType m_textureType;

public:
	Texture(TextureType _type, const char* _imagePath);
	~Texture();
	// Load BMP
	GLuint LoadBMP(const char* _imagePath);
	// Get data
	GLuint GetData();
};

