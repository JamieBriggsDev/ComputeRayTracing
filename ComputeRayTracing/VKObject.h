#if VK
#pragma once

#include "Object.h"

#include <vulkan/vulkan.h>
#include <array>

class VKEngine;

class VKObject :
	public Object
{
private:
	VKEngine* m_vkEngineRef;
public:
	VKObject(VKEngine* _vkEngine,
		const char* _modelFilePath);
	~VKObject();

	void Update(uint32_t _currentSwapChainImage);

	void Draw(Camera* _camera);
};

#endif