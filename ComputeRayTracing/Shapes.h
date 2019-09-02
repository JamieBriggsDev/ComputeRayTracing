#pragma once

#include <glm/glm.hpp>

// Sphere
struct Sphere 
{									// Shader uses std140 layout (so we only use vec4 instead of vec3)
	glm::vec3 pos;
	float radius;
	glm::vec3 diffuse;
	float specular;
	uint32_t id;								// Id used to identify sphere for raytracing
	glm::ivec3 _pad;
};

// Flat Plane
struct Plane 
{
	glm::vec3 normal;
	float distance;
	glm::vec3 diffuse;
	float specular;
	uint32_t id;
	glm::ivec3 _pad;
};

// Creates a new sphere
Sphere newSphere(glm::vec3 pos, float radius, glm::vec3 diffuse, float specular, int &currentID);

// Creates a new plane
Plane newPlane(glm::vec3 normal, float distance, glm::vec3 diffuse, float specular, int &currentID);