#ifdef _WIN32 
#define _CRT_SECURE_NO_DEPRECATE 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Texture.h"


Texture::Texture(TextureType _type, const char* _imagePath)
{
	// Save type
	m_textureType = _type;
	// Load image depending on type
	if (_type == TextureType::BMP)
		m_data = LoadBMP(_imagePath);

}

Texture::~Texture()
{
	glDeleteTextures(1, &m_data); 
}

GLuint Texture::LoadBMP(const char * _imagePath)
{
	// BMP File information
	// BMP files start with 54 byte header
	unsigned char header[54];
	// Position in the file where data begins
	unsigned int dataPosition;
	// Width and height of BMP file
	unsigned int width, height;
	// Image size (width * height * 3)
	unsigned int imageSize;
	// RGB data
	unsigned char* data;

	// Open the file
	FILE* file = fopen(_imagePath, "rb");
	if (!file) 
	{ 
		printf("Image could not be opened\n"); return 0; 
	}
	// Get header
	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}
	// Check if correct file
	if (header[0] != 'B' || header[1] != 'M') 
	{
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Read byte array
	dataPosition = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    
		imageSize = width * height * 3; // 3 - one byte for each Red, Green and Blue component
	if (dataPosition == 0)
		dataPosition = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);


	// Create OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// Bind the newly created texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// pass the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// data not needed anymore
	delete[] data;


	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}

// Get Data.
GLuint Texture::GetData()
{
	return m_data;
}
