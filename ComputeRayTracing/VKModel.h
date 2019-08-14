#if VK

#pragma once

#include <vulkan/vulkan.h>

#include "Model.h"
#include "Vertex.h"
#include "UniformBufferObject.h"

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
	// Uniform Buffers
	std::vector<VkBuffer> m_vkUniformBuffers;
	// Uniform Buffer Memory
	std::vector<VkDeviceMemory> m_vkUniformBuffersMemory;

public:
	// Finds the memory type for creating vertex bufers.
	uint32_t vkFindMemoryType(uint32_t _typeFilter, 
		VkMemoryPropertyFlags _vkProperties);
	// Creates the vertex buffer
	void vkCreateVertexBuffer();
	// Creates the index buffer
	void vkCreateIndexBuffer();
	// Creates the uniform buffers
	void vkCreateUniformBuffers();
	// Cleanup uniform buffers
	void vkCleanupUniformBuffers(int _size);
	// Sets up a buffer helper function.
	void vkSetupBuffer(VkDeviceSize _size,
		VkBufferUsageFlags _usage,
		VkMemoryPropertyFlags _properties,
		VkBuffer& _buffer,
		VkDeviceMemory& _bufferMemory);
	// Update uniform buffer data.
	void vkUpdateUniformBuffer(uint32_t _currentImage, float _deltaTime);
	// Copys buffer
	void vkCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	// Gets Vertex Buffer
	VkBuffer vkGetVertexBuffer() { return m_vkVertexBuffer; }
	// Gets Index Buffer
	VkBuffer vkGetIndexBuffer() { return m_vkIndexBuffer; }
	// Gets Vertices
	std::vector<Vertex> vkGetVertices() { return vertices; }
	// Gets Indices
	std::vector<uint16_t> vkGetIndices() { return indices; }
	// Gets Uniform Buffers
	std::vector<VkBuffer> vkGetUniformBuffers() { return m_vkUniformBuffers; }

	VKModel(VKEngine* _vkEngine);
	~VKModel();
};

#endif