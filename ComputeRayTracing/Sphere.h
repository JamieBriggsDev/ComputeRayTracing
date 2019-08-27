#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera;

class Sphere
{
public:
	// Sphere Position
	glm::vec3 m_position;
	// Sphere Radius
	float m_radius;
	// Sphere Colour
	glm::vec3 m_colour;
public:
	// Get Position
	glm::vec3 GetPosition();
	// Get Radius
	float GetRadius();
	// Get Colour
	glm::vec3 GetColour();

	// Set Position
	void SetPosition(glm::vec3 _pos);
	// Set Radius
	void SetRadius(float _rad);
	// Set Colour
	void SetColour(glm::vec3 _col);

	// Update Sphere TODO
	void Update(float _deltaTime);
};