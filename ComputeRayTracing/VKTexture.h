#if VK

#pragma once
#include "Texture.h"
#include "VKModel.h"
#include "VKEngine.h"

class VKTexture :
	public Texture
{
private:
	// Vulkan Engine Reference
	VKEngine* m_vkEngineRef;
	// Model Reference
	VKModel* m_vkModelRef;
	// Staging buffer
	VkBuffer m_vkStagingBuffer;
	// Staging buffer memory
	VkDeviceMemory m_vkStagingBufferMemory;

	// Texture Image
	VkImage* m_vkTextureImage;
	// Texture Image Memory
	VkDeviceMemory* m_vkTextureImageMemory;

	// Texture Image View
	VkImageView* m_vkTextureImageView;

	// Transition image layout
	void vkTransitionImageLayout(VkFormat _format,
		VkImageLayout _oldLayout,
		VkImageLayout _newLayout);

	// Copy buffer to image
	void vkCopyBufferToImage(uint32_t _width,

		uint32_t _height);

	// Create image helper function
	void CreateImage(uint32_t _width, uint32_t _height, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties);
public:
	// Constructors
	VKTexture(VKModel* _modelRef, VKEngine* _engineRef, const char* _imagePath);
	// Creates the tecture for vulkan
	void CreateTexture(ImageDetails _details);
	// Creates the texture image view
	void CreateTextureImageView();
	// Create image view
	static VkImageView CreateImageView(VkDevice _device, VkImage _image, VkFormat _format);
	// Get Texture Image view
	VkImageView* GetTextureImageView() { return m_vkTextureImageView; }
	
	~VKTexture();
};

#endif