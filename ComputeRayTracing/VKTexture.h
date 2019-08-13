#if VK

#pragma once
#include "Texture.h"
#include "VKModel.h"
#include "VKEngine.h"

class VKTexture :
	public Texture
{
private:
	VKEngine* m_vkEngineRef;
	VKModel* m_vkModelRef;
	VkBuffer m_vkStagingBuffer;
	VkDeviceMemory m_vkStagingBufferMemory;
public:
	// Constructors
	VKTexture(VKModel* _modelRef, VKEngine* _engineRef, const char* _imagePath);
	// Creates the tecture for vulkan
	void CreateTexture(ImageDetails _details);
	~VKTexture();
};

#endif