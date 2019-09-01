#include "Engine.h"


Engine::Engine()
{
	// Setup Spheres
	Sphere one;
	one.SetPosition(glm::vec3(0.5f, 1.0f, -3.5f));
	one.SetRadius(1.0f);
	one.SetColour(glm::vec3(0.0f, 1.0f, 0.0f));
	one.SetMaterial(Material::Diffuse);
	m_spheres.push_back(one);
	Sphere two;
	two.SetPosition(glm::vec3(0.0f, -2.0f, -5.0f));
	two.SetRadius(1.5f);
	two.SetColour(glm::vec3(0.0f, 0.0f, 1.0f));
	two.SetMaterial(Material::Reflective);
	m_spheres.push_back(two);
	// Setup light
	m_light = glm::vec3(-2.0f, 2.0f, 0.0f);
}

Engine::~Engine()
{
	// Terminate GLFW.
	glfwTerminate();
}



