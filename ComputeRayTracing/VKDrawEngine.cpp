#if VK

#include "VKDrawEngine.h"
#include "Window.h"
#include "Camera.h"
#include "Object.h"
#include "VKEngine.h"

// GLFW to handle window and keyboard/ mouse input
#include <GLFW/glfw3.h>


void VKDrawEngine::Update(std::vector<VkCommandBuffer> _commandBuffers)
{
	// Get next image index
	uint32_t imageIndex;
	vkAcquireNextImageKHR(*m_vkEngineRef->vkGetDevice(), 
		*m_vkEngineRef->vkGetSwapChain(), 
		std::numeric_limits<uint64_t>::max(), 
		m_vkImageAvailableSemaphore,
		VK_NULL_HANDLE,
		&imageIndex);

	// Create submit info for queue submission and synchronization.
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	// Wait Semaphores
	VkSemaphore waitSemaphores[] = { m_vkImageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	// Tell submit info about command buffers.
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];
	// Signal Semaphores
	VkSemaphore signalSemaphores[] = { m_vkRenderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	// Submit queue
	if (vkQueueSubmit(*m_vkEngineRef->vkGetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) 
		!= VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	// Present Info
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { *m_vkEngineRef->vkGetSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	// Submit request to present an image to the swap chain
	vkQueuePresentKHR(*m_vkEngineRef->vkGetPresentQueue(), &presentInfo);
	

}

VKDrawEngine::VKDrawEngine(VKEngine* _vkEngine)
{
	// Get reference to vulkan device
	m_vkEngineRef = _vkEngine;
	// Create Semaphores
	vkCreateSemaphores();
	// V sync off 
	glfwSwapInterval(0);
}

VKDrawEngine::~VKDrawEngine()
{
	// Destroy Semaphores.
	vkDestroySemaphore(*m_vkEngineRef->vkGetDevice(), m_vkImageAvailableSemaphore, nullptr);
	vkDestroySemaphore(*m_vkEngineRef->vkGetDevice(), m_vkRenderFinishedSemaphore, nullptr);
}

void VKDrawEngine::vkCreateSemaphores()
{
	// Semaphore create info.
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	// Try create image available and render finished semaphores.
	if (vkCreateSemaphore(*m_vkEngineRef->vkGetDevice(), &semaphoreInfo, nullptr, &m_vkImageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(*m_vkEngineRef->vkGetDevice(), &semaphoreInfo, nullptr, &m_vkRenderFinishedSemaphore) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create semaphores!");
	}

}

#endif