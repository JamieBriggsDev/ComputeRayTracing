#pragma once

#include <glm/glm.hpp>

#include "Material.h"

struct Plane
{
	glm::vec3 normal;
	float distance;

	Material mat;

	Plane();
	Plane(glm::vec3 normal, float distance);
};

struct Sphere
{
	glm::vec3 position;
	float radius;

	Material mat;

	Sphere();
	Sphere(glm::vec3 position, float radius);
};
