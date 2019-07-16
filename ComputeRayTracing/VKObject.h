#if VK
#pragma once

#include "Object.h"
#include <vulkan/vulkan.h>

class VKObject :
	public Object
{
public:
	VKObject(VkDevice _vkDevice, const char* _modelFilePath);
	~VKObject();

	void Draw(Camera* _camera);
};

#endif