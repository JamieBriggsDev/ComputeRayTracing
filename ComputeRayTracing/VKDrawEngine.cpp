#if VK

#include "VKDrawEngine.h"
#include "Window.h"
#include "Camera.h"

#include "VKEngine.h"

// GLFW to handle window and keyboard/ mouse input
#include <GLFW/glfw3.h>


void VKDrawEngine::Update(VKObject* _object, std::vector<VkCommandBuffer> _commandBuffers, Camera* _camera, float _deltaTime)
{
	// Wait for fences
	vkWaitForFences(*m_vkEngineRef->vkGetDevice(), 1, &m_vkInFlightFences[m_currentFrame], 
		VK_TRUE, std::numeric_limits<uint64_t>::max());

	// Get next image index
	uint32_t imageIndex;
	vkAcquireNextImageKHR(*m_vkEngineRef->vkGetDevice(), 
		*m_vkEngineRef->vkGetSwapChain(), 
		std::numeric_limits<uint64_t>::max(), 
		m_vkImageAvailableSemaphore[m_currentFrame],
		VK_NULL_HANDLE,
		&imageIndex);

	// Update object here
	_object->UpdateUniformBuffer(imageIndex, _camera, _deltaTime);


	// Create submit info for queue submission and synchronization.
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	// Wait Semaphores
	VkSemaphore waitSemaphores[] = { m_vkImageAvailableSemaphore[m_currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	// Tell submit info about command buffers.
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];
	// Signal Semaphores
	VkSemaphore signalSemaphores[] = { m_vkRenderFinishedSemaphore[m_currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	// Reset fences
	vkResetFences(*m_vkEngineRef->vkGetDevice(), 1, &m_vkInFlightFences[m_currentFrame]);

	// Submit queue
	if (vkQueueSubmit(*m_vkEngineRef->vkGetGraphicsQueue(), 1, &submitInfo, m_vkInFlightFences[m_currentFrame]) 
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
	
	// Increment frames whilst looping back to frame 0
	m_currentFrame = (m_currentFrame + 1) % PRE_RENDERED_FRAMES;
}

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