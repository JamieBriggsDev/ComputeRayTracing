#if VK

#pragma once

#include "VKEngine.h"

class VKTexture
{
public:
	VkDevice* device;
	VkImage image;
	VkImageLayout imageLayout;
	VkDeviceMemory deviceMemory;
	VkImageView view;
	uint32_t width, height;
	uint32_t mipLevels;
	uint32_t layerCount;
	VkDescriptorImageInfo descriptor;

	// Sampler to be used with the texture.
	VkSampler sampler;

	// Updates the descriptor.
	void UpdateDescriptor();

	// Destroys the texture.
	void Destroy();

	// Set image layout
	static void SetImageLayout(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkImageAspectFlags aspectMask,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask = 65536U,
		VkPipelineStageFlags dstStageMask = 65536U);

	// Set Image layout
	static void SetImageLayout(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkImageSubresourceRange subresourceRange,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask);

	// Create sampler
	void CreateSampler(VkDevice device);
	// Create Image View
	void CreateImageView(VkDevice device, VkFormat format);
};

#endif