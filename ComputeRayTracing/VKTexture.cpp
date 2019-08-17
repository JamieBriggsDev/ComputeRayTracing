#if VK

#include "VKTexture.h"
#include "VKEngine.h"

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
	m_vkTextureImageView = CreateTextureImageView(m_vkTextureImage);
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

	// Create image and memory allocation
	m_vkTextureImage = new VkImage();
	m_vkTextureImageMemory = new VkDeviceMemory();
	// Create image
	VKEngine::vkSetupImage(_details.textureWidth, _details.textureHeight,
		VK_FORMAT_R8G8B8A8_UNORM,
		 VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_vkEngineRef,
		*m_vkTextureImage, *m_vkTextureImageMemory);

	// Transition image layout to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	VKEngine::vkTransitionImageLayout(m_vkEngineRef, *m_vkTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// Copy buffer to image
	vkCopyBufferToImage(static_cast<uint32_t>(_details.textureWidth), static_cast<uint32_t>(_details.textureHeight));

	// Transition texture to prepare for shader access
	VKEngine::vkTransitionImageLayout(m_vkEngineRef, *m_vkTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(*m_vkEngineRef->vkGetDevice(), m_vkStagingBuffer, nullptr);
	vkFreeMemory(*m_vkEngineRef->vkGetDevice(), m_vkStagingBufferMemory, nullptr);
}


VkImageView* VKTexture::CreateTextureImageView(VkImage* _image)
{
	m_vkTextureImageView = new VkImageView();
	*m_vkTextureImageView = CreateImageView(*m_vkEngineRef->vkGetDevice(), *_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	return m_vkTextureImageView;
}


VkImageView VKTexture::CreateImageView(VkDevice _device, VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags)
{
	// Texture Image view create info
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = _image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = _format;
	viewInfo.subresourceRange.aspectMask = _aspectFlags;
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