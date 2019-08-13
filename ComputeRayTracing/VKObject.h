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

	void Update(float _deltaTime);
	void UpdateUniformBuffer(uint32_t _currentSwapChainImage, float _deltaTime);

	void Draw(Camera* _camera, float _deltaTime);
};

#endif