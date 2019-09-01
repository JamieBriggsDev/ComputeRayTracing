#if VK

#include "VKTexture.h"

#include <iostream>

VKTexture::VKTexture(VKModel* _modelRef, VKEngine* _engineRef, const char* _imagePath)
{
	// Keep model and engine reference
	m_vkModelRef = _modelRef;
	m_vkEngineRef = _engineRef;
	// Get image details
	auto details = LoadImage(_imagePath);
	// Bind texture
	CreateTexture(details);
	// Create texture image view.
	CreateTextureImageView();
}


VKTexture::~VKTexture()
{
	// Clean up image view
	vkDestroyImageView(*m_vkEngineRef->vkGetDevice(), *m_vkTextureImageView, nullptr);
	// Clean up texture
	vkDestroyImage(*m_vkEngineRef->vkGetDevice(), *m_vkTextureImage, nullptr);
	// Free memory
	vkFreeMemory(*m_vkEngineRef->vkGetDevice(), *m_vkTextureImageMemory, nullptr);
}

void VKTexture::vkTransitionImageLayout(VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout)
{
	// Start command buffer recording.
	VkCommandBuffer commandBuffer = m_vkEngineRef->vkBeginSingleTimeCommands();

	// Image memory barrier
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = _oldLayout;
	barrier.newLayout = _newLayout;
	// Ignore queue families
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	// Image params
	barrier.image = *m_vkTextureImage;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	// No mipmapping
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;

	// Get source and destination pipeline stage
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}


	// Submit pipeline barrier
	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage,
		destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	// End command buffer recording.
	m_vkEngineRef->vkEndSingleTimeCommands(commandBuffer);
}

void VKTexture::vkCopyBufferToImage(uint32_t _width, uint32_t _height)
{
	// Start command buffer recording.
	VkCommandBuffer commandBuffer = m_vkEngineRef->vkBeginSingleTimeCommands();

	// Vk buffer image copy details
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	// Mipmap details.
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	// Image offset (NONE)
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		_width,
		_height,
		1
	};

	// Copy buffer to the image
	vkCmdCopyBufferToImage(
		commandBuffer,
		m_vkStagingBuffer,
		*m_vkTextureImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	// End command buffer recording.
	m_vkEngineRef->vkEndSingleTimeCommands(commandBuffer);
}

void VKTexture::CreateImage(uint32_t _width, uint32_t _height, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties)
{
	// Image create info
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(_width);
	imageInfo.extent.height = static_cast<uint32_t>(_height);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	// Image format
	imageInfo.format = _format;
	// Tiling mode
	imageInfo.tiling = _tiling;
	// Image layout
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Image usage
	imageInfo.usage = _usage;
	// Image sharing mode
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// Image samples
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	// flags
	imageInfo.flags = 0;

	// Create the image
	m_vkTextureImage = new VkImage();
	if (vkCreateImage(*m_vkEngineRef->vkGetDevice(), &imageInfo, nullptr, m_vkTextureImage) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}

	// Allocate texture image memory
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(*m_vkEngineRef->vkGetDevice(),
		*m_vkTextureImage,
		&memRequirements);

	// Allocate info
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	// Find memory type
	allocInfo.memoryTypeIndex = m_vkModelRef->vkFindMemoryType(memRequirements.memoryTypeBits,
		_properties);
	// Allocate memory
	m_vkTextureImageMemory = new VkDeviceMemory();
	if (vkAllocateMemory(*m_vkEngineRef->vkGetDevice(), &allocInfo, nullptr, m_vkTextureImageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}
	// Bind image to memory.
	vkBindImageMemory(*m_vkEngineRef->vkGetDevice(),
		*m_vkTextureImage,
		*m_vkTextureImageMemory, 0);
}



void VKTexture::CreateTexture(ImageDetails _details)
{
	// Create buffer
	m_vkModelRef->vkSetupBuffer(m_textureSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		m_vkStagingBuffer,
		m_vkStagingBufferMemory);
	// Copy pixel values to the buffer
	void* data;
	vkMapMemory(*m_vkEngineRef->vkGetDevice(),
		m_vkStagingBufferMemory,
		0,
		m_textureSize,
		0,
		&data);
	memcpy(data, _details.pixels, static_cast<size_t>(m_textureSize));
	vkUnmapMemory(*m_vkEngineRef->vkGetDevice(), m_vkStagingBufferMemory);

	// Create image
	CreateImage(_details.textureWidth, _details.textureHeight,
		VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// Transition image layout to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	vkTransitionImageLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// Copy buffer to image
	vkCopyBufferToImage(static_cast<uint32_t>(_details.textureWidth), static_cast<uint32_t>(_details.textureHeight));

	// Transition texture to prepare for shader access
	vkTransitionImageLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(*m_vkEngineRef->vkGetDevice(), m_vkStagingBuffer, nullptr);
	vkFreeMemory(*m_vkEngineRef->vkGetDevice(), m_vkStagingBufferMemory, nullptr);
}


void VKTexture::CreateTextureImageView()
{
	m_vkTextureImageView = new VkImageView();
	*m_vkTextureImageView = CreateImageView(*m_vkEngineRef->vkGetDevice(), *m_vkTextureImage, VK_FORMAT_R8G8B8A8_UNORM);
}


VkImageView VKTexture::CreateImageView(VkDevice _device, VkImage _image, VkFormat _format)
{
	// Texture Image view create info
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = _image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = _format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	// Create the image view
	VkImageView TextureImageView;
	if (vkCreateImageView(_device, &viewInfo, nullptr, &TextureImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture image view!");
	}

	return TextureImageView;
}

#endif