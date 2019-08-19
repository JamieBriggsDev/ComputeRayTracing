#if VK

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "VKModel.h"
#include "VKEngine.h"


VKModel::VKModel(VKEngine* _vkEngine, const char* _objFilePath)
	: Model(_objFilePath)
{
	vertices = m_verticesData;
	indices = m_indicesBufferData;

	// Reference device.
	m_vkEngineRef = _vkEngine;
	// Setup vertex buffers.
	vkCreateVertexBuffer();
	// Setup index buffers.
	vkCreateIndexBuffer();
	// Setup uniform buffers
	vkCreateUniformBuffers();
}


VKModel::~VKModel()
{
	// Destroy vertex buffer.
	vkDestroyBuffer(*m_vkEngineRef->vkGetDevice(), m_vkVertexBuffer, nullptr);
	// Free vertex memory.
	vkFreeMemory(*m_vkEngineRef->vkGetDevice(), m_vkVertexBufferMemory, nullptr);
	// Destroy index buffer.
	vkDestroyBuffer(*m_vkEngineRef->vkGetDevice(), m_vkIndexBuffer, nullptr);
	// Free index memory.
	vkFreeMemory(*m_vkEngineRef->vkGetDevice(), m_vkIndexBufferMemory, nullptr);
	// Destroy uniform buffers.
	vkCleanupUniformBuffers(m_vkEngineRef->vkGetSwapChainImages().size());
}

void VKModel::vkCreateVertexBuffer()
{
	// Get Buffer Size
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	// Setup Stage buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	vkSetupBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, 
		stagingBufferMemory);

	// Store vertices data.
	void* data;
	vkMapMemory(*m_vkEngineRef->vkGetDevice(), 
		stagingBufferMemory,
		0, 
		bufferSize, 
		0, 
		&data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(*m_vkEngineRef->vkGetDevice(), stagingBufferMemory);

	// Setup Buffer
	vkSetupBuffer(bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_vkVertexBuffer,
		m_vkVertexBufferMemory);

	// Copy stafing buffer to vertex buffer.
	vkCopyBuffer(stagingBuffer, m_vkVertexBuffer, bufferSize);
	// Cleanup staging buffer.
	vkDestroyBuffer(*m_vkEngineRef->vkGetDevice(), stagingBuffer, nullptr);
	vkFreeMemory(*m_vkEngineRef->vkGetDevice(), stagingBufferMemory, nullptr);

}

void VKModel::vkCreateIndexBuffer()
{
	// Find index buffer size.
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	// Setup staging buffer.
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	vkSetupBuffer(bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, 
		stagingBufferMemory);

	// Copy staging data.
	void* data;
	vkMapMemory(*m_vkEngineRef->vkGetDevice(), 
		stagingBufferMemory,
		0, 
		bufferSize,
		0,
		&data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(*m_vkEngineRef->vkGetDevice(), stagingBufferMemory);
	// Setup index buffer.
	vkSetupBuffer(bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT |
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		m_vkIndexBuffer, 
		m_vkIndexBufferMemory);
	// Transfer staging buffer into index buffer.
	vkCopyBuffer(stagingBuffer, m_vkIndexBuffer, bufferSize);

	// Cleanup index buffer.
	vkDestroyBuffer(*m_vkEngineRef->vkGetDevice(), stagingBuffer, nullptr);
	vkFreeMemory(*m_vkEngineRef->vkGetDevice(), stagingBufferMemory, nullptr);
}

void VKModel::vkSetupBuffer(VkDeviceSize _size,
	VkBufferUsageFlags _usage,
	VkMemoryPropertyFlags _properties,
	VkBuffer& _buffer,
	VkDeviceMemory& _bufferMemory) 
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = _size;
	bufferInfo.usage = _usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	// Save temp device.
	VkDevice* tempDevice = m_vkEngineRef->vkGetDevice();

	if (vkCreateBuffer(*tempDevice, &bufferInfo, nullptr, &_buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(*tempDevice, _buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VKEngine::vkFindMemoryType(memRequirements.memoryTypeBits, _properties, m_vkEngineRef);

	if (vkAllocateMemory(*tempDevice, &allocInfo, nullptr, &_bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(*tempDevice, _buffer, _bufferMemory, 0);
}

void VKModel::vkCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Start command buffer recording.
	VkCommandBuffer commandBuffer = m_vkEngineRef->vkBeginSingleTimeCommands();

	// Transfer source buffer to destination buffer.
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	// End command buffer
	m_vkEngineRef->vkEndSingleTimeCommands(commandBuffer);
}

void VKModel::vkCreateUniformBuffers()
{
	// Gets the uniform buffer object size.
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	// Resizes the uniform buffers to the same size as the swap chain images.
	m_vkUniformBuffers.resize(m_vkEngineRef->vkGetSwapChainImages().size());
	m_vkUniformBuffersMemory.resize(m_vkEngineRef->vkGetSwapChainImages().size());
	// Sets up the individual buffers.
	for (size_t i = 0; i < m_vkEngineRef->vkGetSwapChainImages().size(); i++) {
		vkSetupBuffer(bufferSize, 
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			m_vkUniformBuffers[i], 
			m_vkUniformBuffersMemory[i]);
	}
}

void VKModel::vkCleanupUniformBuffers(int _size)
{
	// Destroys all the uniform buffers and its memory
	//  counterparts.
	for (size_t i = 0; i < _size; i++) 
	{
		vkDestroyBuffer(*m_vkEngineRef->vkGetDevice(), 
			m_vkUniformBuffers[i], 
			nullptr);
		vkFreeMemory(*m_vkEngineRef->vkGetDevice(),
			m_vkUniformBuffersMemory[i], 
			nullptr);
	}
}

void VKModel::vkUpdateUniformBuffer(uint32_t _currentImage, Camera* _camera, float _deltaTime)
{
	// Init timer
	static float Timer;
	if (Timer == NULL)
		Timer = _deltaTime;
	else
		Timer += _deltaTime;

	// Update ubo TODO - MOVE TO VKOBJECT
	//UniformBufferObject ubo = {};
	//ubo.m_model = glm::rotate(glm::mat4(1.0f), Timer * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//ubo.m_view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//ubo.m_proj = glm::perspective(glm::radians(45.0f), 
	//	m_vkEngineRef->vkGetSwapChainExtent().width / 
	//	(float)m_vkEngineRef->vkGetSwapChainExtent().height, 
	//	0.1f, 10.0f);
	UniformBufferObject ubo = {};
	ubo.m_model = glm::rotate(glm::mat4(1.0f), Timer * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.m_view = _camera->GetView();
	ubo.m_proj = _camera->GetProjection();

	ubo.m_proj[1][1] *= -1;

	void* data;
	vkMapMemory(*m_vkEngineRef->vkGetDevice(), 
		m_vkUniformBuffersMemory[_currentImage], 
		0, 
		sizeof(ubo), 
		0, 
		&data);

	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(*m_vkEngineRef->vkGetDevice(),
		m_vkUniformBuffersMemory[_currentImage]);
}

#endif