#include "Shapes.h"


Sphere::Sphere() { radius = 1; }
Sphere::Sphere(glm::vec3 position, float radius) : position(position), radius(radius) {}

Plane::Plane() { distance = 1; }
Plane::Plane(glm::vec3 normal, float distance) : normal(normal), distance(distance) {}