#if VK

#pragma once
#include "Object.h"
class VKObject :
	public IObject
{
public:
	VKObject(VkDevice _vkDevice, const char* _modelFilePath);
	~VKObject();

	void Draw(Camera* _camera);
};

#endif