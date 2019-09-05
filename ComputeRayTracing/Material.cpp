#include "Material.h"

Material::Material() { color = glm::vec3(0, 0, 0);  type = 1; }
Material::Material(glm::vec3 color, int type) : color(color), type(type) {}