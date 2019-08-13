#pragma once

#include <GL/glew.h>
#include <stdexcept>


struct ImageDetails {
public:
	int textureWidth;
	int textureHeight;
	int textureChannels;
	unsigned char* pixels;
};

enum class TextureType {BMP};

class Texture
{
protected:
	GLuint m_data;
	TextureType m_textureType;
	size_t m_textureSize;

public:
	Texture() {};
	Texture(TextureType _type, const char* _imagePath);
	~Texture();
	// Load BMP
	GLuint LoadBMP(const char* _imagePath);
	// Load Image
	ImageDetails LoadImage(const char* _imagePath);
	// Get data
	GLuint GetData();
};

