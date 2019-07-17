#if VK

#pragma once

#include "Model.h"

#include <vulkan/vulkan.h>

#include "Vertex.h"

class VKEngine;

class VKModel :
	public Model
{
private:
	const std::vector<Vertex> vertices = {
		{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 0.0f}}
	};
	// Vulkan Device Reference.
	VKEngine* m_vkEngineRef;
	// Vertex Buffer.
	VkBuffer m_vkVertexBuffer;
	// Vertx Buffer Memory.
	VkDeviceMemory m_vkVertexBufferMemory;

	// Finds the memory type for creating vertex bufers.
	uint32_t vkFindMemoryType(uint32_t _typeFilter, 
		VkMemoryPropertyFlags _vkProperties);
public:
	void vkCreateVertexBuffer();
	void vkSetupBuffer(VkDeviceSize _size,
		VkBufferUsageFlags _usage,
		VkMemoryPropertyFlags _properties,
		VkBuffer& _buffer,
		VkDeviceMemory& _bufferMemory);

	VkBuffer vkGetVertexBuffer() { return m_vkVertexBuffer; }
	std::vector<Vertex> vkGetVertices() { return vertices; }
	void vkCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VKModel(VKEngine* _vkEngine);
	~VKModel();
};

#endif