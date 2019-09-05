#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::vec3 color;
	int type;

	Material();
	Material(glm::vec3, int);
};