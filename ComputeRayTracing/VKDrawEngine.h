#if VK

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class Object;
class Camera;
class Window;
class VKEngine;

class VKDrawEngine
{
private:
	// Reference to vulkan device.
	VKEngine* m_vkEngineRef;
	VkSemaphore m_vkImageAvailableSemaphore;
	VkSemaphore m_vkRenderFinishedSemaphore;
	// Create semaphore signals
	void vkCreateSemaphores();
public:
	void Update(std::vector<VkCommandBuffer> _commandBuffers);
	VKDrawEngine(VKEngine* _vkEngine);
	~VKDrawEngine();
};

#endif