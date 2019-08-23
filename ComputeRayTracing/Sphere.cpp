#include "Sphere.h"

glm::vec3 Sphere::GetPosition()
{
	return m_position;
}

float Sphere::GetRadius()
{
	return m_radius;
}

glm::vec3 Sphere::GetColour()
{
	return m_colour;
}

void Sphere::SetPosition(glm::vec3 _pos)
{
	// Set Position
	m_position = _pos;
}

void Sphere::SetRadius(float _rad)
{
	// Set Radius
	m_radius = _rad;
}

void Sphere::SetColour(glm::vec3 _col)
{
	// Set Colour
	m_colour = _col;
}

void Sphere::Update(float _deltaTime)
{
	//Update Position
}
