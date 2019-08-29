#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera;

class Sphere
{
private:
	// Sphere Radius
	float m_radius;
	// Sphere Colour
	glm::vec3 m_colour;
	// Count
	float m_count = 0.f;
public:
	// Sphere Position
	glm::vec3 m_position;
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