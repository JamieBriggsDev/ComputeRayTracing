#pragma once

#include <GL/glew.h>

static const GLfloat g_screenSpaceVertices[] = {
   1.0f, -1.0f, 0.0f,
   1.0f, 1.0f, 0.0f,
   -1.0f, 1.0f, 0.0f,
   -1.0, -1.0f, 0.0f
};

static const GLint g_screenSpaceIndices[] = {
	0, 1, 3, 3, 1, 2
};

static const GLfloat  g_screenSpaceUVs[] = {
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f
};