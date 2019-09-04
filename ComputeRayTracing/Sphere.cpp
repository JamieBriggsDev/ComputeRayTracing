#if GL

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

Material Sphere::GetMaterial()
{
	return m_material;
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

void Sphere::SetMaterial(Material _material)
{
	m_material = _material;
}

void Sphere::Update(float _deltaTime)
{
	
	//static glm::vec3 orig = m_position;
	m_count += _deltaTime;

	//Update Position
	//m_position.x = sin(m_count);// +m_position.x;
	//m_position.y = cos(m_count);// +m_position.y;
}

#endif