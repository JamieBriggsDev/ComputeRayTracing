#if GL

#include "GLDrawEngine.h"
#include "GLModel.h"
#include "GLPipeline.h"
#include "HardModels.h"
#include "Camera.h"
#include "Window.h"
#include "Engine.h"

// GLEW before everything else
#include <GL/glew.h>

// GLFW to handle window and keyboard/ mouse input
#include <GLFW/glfw3.h>



void GLDrawEngine::Update(Camera* _camera, Window* _window, Object* _object, float _deltaTime)
{

	//// Compute shader stuff first
	//glUseProgram(m_pipeline->GetComputeProgramID());
	//// Define groups (Window Resolution)
	//glDispatchCompute((GLuint)Window::s_windowWidth, (GLuint)Window::s_windowHeight, 1);
	//// Make sure writing to image has finished before read
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Use frag/vert shader program 
	glUseProgram(m_pipeline->GetProgramID());
	// MVP
	//glm::mat4 MVP = _camera->GetProjectionView() * _object->GetModelMatrix();

	// Send our transformations to the shader
	//glUniformMatrix4fv(static_cast<GLPipeline*>(_object->GetPipeline())->GetMVPID(), 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UV
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);


	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_indexBuffer);

	// Draw the triangle !
	//glDrawArrays(GL_TRIANGLES, 0, m_model->GetIndicesCount());
	glDrawElements(
		GL_TRIANGLES,
		static_cast<GLModel*>(_object->GetModel())->GetIndicesCount(),
		GL_UNSIGNED_INT,
		(void*)0
	);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);


}

void GLDrawEngine::CreateScreenSpace()
{
	// Bind IDs
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Bind Vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_screenSpaceVertices),
		&g_screenSpaceVertices[0], GL_STATIC_DRAW);
	// Bind UV buffer
	glGenBuffers(1, &m_uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_screenSpaceUVs), 
		&g_screenSpaceUVs[0], GL_STATIC_DRAW);
	// Bind Element/ Index Buffer
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_screenSpaceIndices),
		&g_screenSpaceIndices[0], GL_STATIC_DRAW);
}

GLDrawEngine::GLDrawEngine()
{
	// Load Shaders
	m_pipeline = new GLPipeline("Shaders/GLShader.vert",
		"Shaders/GLShader.frag",
		"Shaders/GLShader.comp");
	// light blue background
	glClearColor(OPENGL_CLEAR_COLOUR);

	// Enable face culling
	glEnable(GL_CULL_FACE);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// V sync off 
	glfwSwapInterval(0);

	// Create Screen Space
	CreateScreenSpace();
}


GLDrawEngine::~GLDrawEngine()
{
}

#endif