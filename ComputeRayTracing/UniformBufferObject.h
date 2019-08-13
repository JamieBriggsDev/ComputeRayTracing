#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

struct UniformBufferObject
{
public:
	glm::mat4 m_model;
	glm::mat4 m_view;
	glm::mat4 m_proj;
};

