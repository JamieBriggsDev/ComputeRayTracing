#if VK
#pragma once

#include "Object.h"

#include <vulkan/vulkan.h>
#include <array>

class VKEngine;

class VKObject :
	public Object
{
public:
	VKObject(VKEngine* _vkEngine,
		const char* _modelFilePath);
	~VKObject();

	void Draw(Camera* _camera);
};

#endif