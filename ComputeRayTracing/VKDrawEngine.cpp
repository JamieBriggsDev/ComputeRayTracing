#if VK

#include "VKDrawEngine.h"
#include "Window.h"
#include "VKEngine.h"

// GLFW to handle window and keyboard/ mouse input
#include <GLFW/glfw3.h>


VKDrawEngine::VKDrawEngine(VKEngine* _vkEngine)
{
	// Get reference to vulkan device
	m_vkEngineRef = _vkEngine;
	// Create Semaphores
	vkCreateSyncObjects();
	// V sync off 
	glfwSwapInterval(0);
}

VKDrawEngine::~VKDrawEngine()
{
	// Destroy Semaphores.
	for (size_t i = 0; i < PRE_RENDERED_FRAMES; i++)
	{
		vkDestroySemaphore(*m_vkEngineRef->vkGetDevice(), m_vkImageAvailableSemaphore[i], nullptr);
		vkDestroySemaphore(*m_vkEngineRef->vkGetDevice(), m_vkRenderFinishedSemaphore[i], nullptr);
		vkDestroyFence(*m_vkEngineRef->vkGetDevice(), m_vkInFlightFences[i], nullptr);
	}
}

void VKDrawEngine::vkCreateSyncObjects()
{
	m_vkImageAvailableSemaphore.resize(PRE_RENDERED_FRAMES);
	m_vkRenderFinishedSemaphore.resize(PRE_RENDERED_FRAMES);
	m_vkInFlightFences.resize(PRE_RENDERED_FRAMES);

	// Semaphore create info.
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	// Fence create info.
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < PRE_RENDERED_FRAMES; i++)
	{
		// Try create image available and render finished semaphores.
		if (vkCreateSemaphore(*m_vkEngineRef->vkGetDevice(), &semaphoreInfo, nullptr, &m_vkImageAvailableSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(*m_vkEngineRef->vkGetDevice(), &semaphoreInfo, nullptr, &m_vkRenderFinishedSemaphore[i]) != VK_SUCCESS ||
			vkCreateFence(*m_vkEngineRef->vkGetDevice(), &fenceInfo, nullptr, &m_vkInFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create synchronization frame objects!");
		}
	}

}

#endif