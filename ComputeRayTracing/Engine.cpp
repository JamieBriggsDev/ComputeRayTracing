#include "Engine.h"


#if VK
void Engine::InitialiseObjects(std::vector<Plane> &_planes, std::vector<Sphere> &_spheres)
{
	auto AddPlane = [&_planes](Plane* go, glm::vec3 color, int type)
	{
		go->mat = Material(color, type);
		_planes.push_back(*go);
	};

	auto AddSphere = [&_spheres](Sphere* go, glm::vec3 color, int type)
	{
		go->mat = Material(color, type);
		_spheres.push_back(*go);
	};

	auto sphere = new Sphere(glm::vec3(0.0, -1.55, -4.0), 1.0);
	auto sphere2 = new Sphere(glm::vec3(-1.3, 1.2, -4.2), 0.8);

	auto bottom = new Plane(glm::vec3(0, 1, 0), 2.5);
	auto back = new Plane(glm::vec3(0, 0, 1), 5.5);
	auto left = new Plane(glm::vec3(1, 0, 0), 2.75);
	auto right = new Plane(glm::vec3(-1, 0, 0), 2.75);
	auto ceiling = new Plane(glm::vec3(0, -1, 0), 3.0);
	auto front = new Plane(glm::vec3(0, 0, -1), 0.5);


	AddSphere(sphere, glm::vec3(0.3, 0.9, 0.76), REFLECT_TYPE);
	AddSphere(sphere2, glm::vec3(0.062, 0.917, 0.078), REFLECT_TYPE);

	AddPlane(bottom, glm::vec3(0.8, 0.8, 0.8), DIFFUSE_TYPE);
	AddPlane(left, glm::vec3(1, 0.0, 0.0), DIFFUSE_TYPE);
	AddPlane(right, glm::vec3(0.0, 0.0, 1.0), DIFFUSE_TYPE);
	AddPlane(ceiling, glm::vec3(0.0, 1.0, 0.0), DIFFUSE_TYPE);
	AddPlane(back, glm::vec3(0.8, 0.8, 0.8), DIFFUSE_TYPE);
	AddPlane(front, glm::vec3(1.0, 0.8, 0.0), DIFFUSE_TYPE);
}
#endif

Engine::Engine()
{
#if GL
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
#endif // TODO REMOVE!!!
}

Engine::~Engine()
{
	// Terminate GLFW.
	glfwTerminate();
}



