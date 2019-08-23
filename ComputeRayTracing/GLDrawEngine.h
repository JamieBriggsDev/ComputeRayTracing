#if GL
#pragma once

#include <GL/glew.h>
#include "GLPipeline.h"
#include "Sphere.h"

class Object;
class Camera;
class Window;

class GLDrawEngine
{
private:
	GLuint m_vertexArrayID;
	GLuint m_vertexBuffer;
	GLuint m_indexBuffer;
	GLuint m_uvBuffer;

	// Shader
	GLPipeline* m_pipeline;
public:
	void Update(Camera* _camera, Window* _window, 
		std::vector<Sphere> _spheres, float _deltaTime);

	void CreateScreenSpace();
	GLDrawEngine();
	~GLDrawEngine();
};

#endif