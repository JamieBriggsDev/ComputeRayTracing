#if VK
#pragma once

#include "Object.h"
#include <vulkan/vulkan.h>

class VKObject :
	public Object
{
public:
	VKObject(VkDevice* _vkDevice, 
		VkExtent2D _vkExtent, 
		VkFormat _vkSwapChainFormat,
		const char* _modelFilePath);
	~VKObject();

	void Draw(Camera* _camera);
};

#endif