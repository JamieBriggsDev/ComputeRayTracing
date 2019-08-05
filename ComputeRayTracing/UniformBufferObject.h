#pragma once

#include <glm/glm.hpp>

struct UniformBufferObject
{
public:
	glm::mat4 m_model;
	glm::mat4 m_view;
	glm::mat4 m_proj;
};

