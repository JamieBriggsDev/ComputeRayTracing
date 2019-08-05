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
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
	};
	// Vulkan Device Reference.
	VKEngine* m_vkEngineRef;
	// Vertex Buffer.
	VkBuffer m_vkVertexBuffer;
	// Vertex Buffer Memory.
	VkDeviceMemory m_vkVertexBufferMemory;
	// Index Buffer.
	VkBuffer m_vkIndexBuffer;
	// Index Buffer Memory.
	VkDeviceMemory m_vkIndexBufferMemory;

	// Finds the memory type for creating vertex bufers.
	uint32_t vkFindMemoryType(uint32_t _typeFilter, 
		VkMemoryPropertyFlags _vkProperties);
public:
	void vkCreateVertexBuffer();
	void vkCreateIndexBuffer();
	void vkSetupBuffer(VkDeviceSize _size,
		VkBufferUsageFlags _usage,
		VkMemoryPropertyFlags _properties,
		VkBuffer& _buffer,
		VkDeviceMemory& _bufferMemory);

	VkBuffer vkGetVertexBuffer() { return m_vkVertexBuffer; }
	VkBuffer vkGetIndexBuffer() { return m_vkIndexBuffer; }
	std::vector<Vertex> vkGetVertices() { return vertices; }
	std::vector<uint16_t> vkGetIndices() { return indices; }
	void vkCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VKModel(VKEngine* _vkEngine);
	~VKModel();
};

#endif