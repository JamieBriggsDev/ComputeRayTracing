#if VK

#include "VKModel.h"

#include "VKEngine.h"


VKModel::VKModel(VKEngine* _vkEngine)
{
	// Reference device.
	m_vkEngineRef = _vkEngine;
	// Setup vertex buffers.
	vkCreateVertexBuffer();
	// Setup index buffers.
	vkCreateIndexBuffer();
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
}

uint32_t VKModel::vkFindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _vkProperties)
{
	// Physical device memory properties container.
	VkPhysicalDeviceMemoryProperties memProperties;
	// Get Reuquirements.
	vkGetPhysicalDeviceMemoryProperties(*m_vkEngineRef->vkGetPhysicalDevice(),
		&memProperties);
	// Loop through memory types.
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
	{
		if ((_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _vkProperties)
			== _vkProperties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
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
	allocInfo.memoryTypeIndex = vkFindMemoryType(memRequirements.memoryTypeBits, _properties);

	if (vkAllocateMemory(*tempDevice, &allocInfo, nullptr, &_bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(*tempDevice, _buffer, _bufferMemory, 0);
}

void VKModel::vkCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	// Setup command buffer to transfer buffer
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = *m_vkEngineRef->vkGetCommandPool();
	allocInfo.commandBufferCount = 1;
	// Setup Command buffer
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(*m_vkEngineRef->vkGetDevice(), &allocInfo, &commandBuffer);

	// Setup recording command buffer.
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	// Start command buffer.
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	// Transfer source buffer to destination buffer.
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	// End command buffer.
	vkEndCommandBuffer(commandBuffer);

	// Submit command buffer.
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(*m_vkEngineRef->vkGetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(*m_vkEngineRef->vkGetGraphicsQueue());
}

#endif