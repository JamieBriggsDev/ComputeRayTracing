#pragma once

#include <glm/glm.hpp>


#if VK
#include <vulkan/vulkan.h>
#include <array>

#endif

struct Vertex
{
#if GL
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
#elif VK
	glm::vec2 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
#endif

	bool operator==(const Vertex& other) const 
	{
		return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
	}
#if VK
	// Get binding description
	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	// Define array containing layout information
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
#endif
};
