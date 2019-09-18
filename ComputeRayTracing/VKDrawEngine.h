#if VK

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

const int PRE_RENDERED_FRAMES = 1;

class VKObject;
class Camera;
class Window;
class VKEngine;

class VKDrawEngine
{
private:
	// Reference to vulkan device.
	VKEngine* m_vkEngineRef;
	// Semaphores for each pre rendered frame.
	std::vector<VkSemaphore> m_vkImageAvailableSemaphore;
	std::vector<VkSemaphore> m_vkRenderFinishedSemaphore;
	std::vector<VkFence> m_vkInFlightFences;
	// Frame Count
	size_t m_currentFrame = 0;
	// Create semaphore signals
	void vkCreateSyncObjects();
public:
	VKDrawEngine(VKEngine* _vkEngine);
	~VKDrawEngine();
};

#endif