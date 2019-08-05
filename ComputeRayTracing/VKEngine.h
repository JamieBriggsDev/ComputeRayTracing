#pragma once
#if VK

#include "Engine.h"
#include "VKDrawEngine.h"

// Vulkan
#include <vulkan/vulkan.h>
// Optional
#include <optional>


// Vulkan Extensions
const std::vector<const char*> vkDeviceExtensions = {
VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
// Queue Family Indices
struct vkQueueFamilyIndices
{
	std::optional<uint32_t> m_vkGraphicsFamily;
	std::optional<uint32_t> m_vkPresentFamily;

	bool isComplete()
	{
		return m_vkGraphicsFamily.has_value() && m_vkPresentFamily.has_value();
	}
};
// Swap Chain Support Details
struct vkSwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR m_vkCapabilities;
	std::vector<VkSurfaceFormatKHR> m_vkFormats;
	std::vector<VkPresentModeKHR> m_vkPresentModes;
};

class VKEngine :
	public Engine
{
private:
	// VKDraw Engine
	VKDrawEngine* m_myDrawEngine;

	// Vulkan Instace
	VkInstance* m_vkInstance;
	// Vulkan Physical Device
	VkPhysicalDevice* m_vkPhysicalDevice;
	// Vulkan Logical Device
	VkDevice* m_vkDevice;
	// Debug Messenger
	VkDebugUtilsMessengerEXT* m_vkDebugMessenger;
	// Graphics Queue
	VkQueue* m_vkGraphicsQueue;
	// Present Queue
	VkQueue* m_vkPresentQueue;
	// Window Surface
	VkSurfaceKHR* m_vkSurface;

	// Swap chain
	VkSwapchainKHR* m_vkSwapChain;
	// Swap chain images
	std::vector<VkImage> m_vkSwapChainImages;
	// Swap chain format
	VkFormat m_vkSwapChainImageFormat;
	// Swap chain extent
	VkExtent2D m_vkSwapChainExtent;
	// Swap chain images
	std::vector<VkImageView> m_vkSwapChainImageViews;
	// Swap chain buffer images
	std::vector<VkFramebuffer> m_vkSwapChainFrameBuffers;

	// Command Pool
	VkCommandPool* m_vkCommandPool;
	// Command Buffers
	std::vector<VkCommandBuffer> m_vkCommandBuffers;



	// Vulkan Engine Init Code;
	void Initialise();
	// Vulkan Engine Main Loop
	void MainLoop();

	// Populate Debug Messenger Create Info.
	void vkPopulateDebugMessengerCreateInfo
		(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	// Setup Debug Messenger.
	void vkSetupDebugMessenger(VkInstance* _vkInstance, VkDebugUtilsMessengerEXT* _vkMessenger);
	// Checks support for swap chains.
	vkSwapChainSupportDetails vkQuerySwapChainSupport(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface);
	// Finds queue families.
	vkQueueFamilyIndices vkFindQueueFamilies(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface);
	// Choose best suitable swap surface format.
	VkSurfaceFormatKHR vkChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats);
	// Chose best suitable swap present mode.
	VkPresentModeKHR vkChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes);
	// Choose best suitable swap extent.
	VkExtent2D vkChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities);
	// Create Swap Chain.
	void vkCreateSwapChain(VkDevice* _vkDevice, VkPhysicalDevice* _vkPhysicalDevice,
		VkSurfaceKHR* _vkSurface, VkSwapchainKHR* _vkSwapChain);
	// Get Reuiqred Extensions.
	std::vector<const char*> vkGetRequiredExtensions();
	// Check Device Extension Support.
	bool vkCheckDeviceExtensionSupport(VkPhysicalDevice _vkDevice);
	// Checks if Device is suitable.
	bool vkIsDeviceSuitable(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface);
	// Picks a physical Device.
	void vkPickPhysicalDevice(VkInstance* _vkInstance, VkPhysicalDevice* _vkPhysicalDevice, VkSurfaceKHR* _vkSurface);
	// Creates the Logical Device.
	void vkCreateLogicalDevice(VkPhysicalDevice* _vkPhysicalDevice, VkDevice* _vkDevice,
		VkQueue* _vkGraphicsQueue, VkSurfaceKHR* _vkSurface);
	// Creates the surface.
	void vkCreateSurface(VkInstance* _vkInsance, GLFWwindow* _window, VkSurfaceKHR* _vkSurface);
	// Creates the image views.
	void vkCreateImageViews(VkDevice* _vkDevice,
		std::vector<VkImageView>& _vkSwapChainImageViews,
		std::vector<VkImage>& _vkSwapChainImages,
		VkFormat* _vkSwapChainImageFormat);
	// Create Frame Buffers.
	void vkCreateFrameBuffers();
	// Create Command Pool.
	void vkSetupCommandPool();
	// Setup Command Buffers.
	void vkSetupCommandBuffers();
public:
	VKEngine();
	~VKEngine();

	// Getter Functions
	// Get Vulkan Device
	VkDevice* vkGetDevice() { return m_vkDevice; }
	// Get Graphics Queue
	VkQueue* vkGetGraphicsQueue() { return m_vkGraphicsQueue; }
	// Get Present Queue
	VkQueue* vkGetPresentQueue() { return m_vkPresentQueue; }
	// Get Swap chain
	VkSwapchainKHR* vkGetSwapChain() { return m_vkSwapChain; }
	// Get Command Buffers
	std::vector<VkCommandBuffer> vkGetCommandBuffers() { return m_vkCommandBuffers; }

};

#endif