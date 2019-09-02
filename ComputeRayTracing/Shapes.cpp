#include "Shapes.h"

Sphere newSphere(glm::vec3 pos, float radius, glm::vec3 diffuse, float specular, int & currentID)
{
	Sphere sphere;
	sphere.id = currentID++;
	sphere.pos = pos;
	sphere.radius = radius;
	sphere.diffuse = diffuse;
	sphere.specular = specular;
	return sphere;
}

Plane newPlane(glm::vec3 normal, float distance, glm::vec3 diffuse, float specular, int & currentID)
{
	Plane plane;
	plane.id = currentID++;
	plane.normal = normal;
	plane.distance = distance;
	plane.diffuse = diffuse;
	plane.specular = specular;
	return plane;
}
