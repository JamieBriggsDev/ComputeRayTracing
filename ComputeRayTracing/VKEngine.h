#pragma once
#if VK

#include "Engine.h"
#include "VKDrawEngine.h"
// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

	// Depth Image
	VkImage* m_vkDepthImage;
	// Depth image memory
	VkDeviceMemory* m_vkDepthImageMemory;
	// Depth image view
	VkImageView* m_vkDepthImageView;

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
	void vkCreateCommandBuffers();
	// Setup depth buffer resources.
	void vkSetupDepthBufferResources();
	// Find depth format.
	VkFormat vkFindDepthFormat();
	// Check for stentil component.
	bool vkHasStencilComponent(VkFormat format);
	// Finds supported format.
	VkFormat vkFindSupportedFormat(const std::vector<VkFormat>& candidates,
		VkImageTiling tiling,
		VkFormatFeatureFlags features);
public:
	VKEngine();
	~VKEngine();

	// Static helper functions.
	// Find memory type
	static uint32_t vkFindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _vkProperties, VKEngine* _vkEngine);
	// Create Image
	static void vkSetupImage(uint32_t _width,
		uint32_t _height,
		VkFormat _format,
		VkImageTiling _tiling,
		VkImageUsageFlags _usage,
		VkMemoryPropertyFlags _properties,
		VKEngine* _vkEngine, VkImage &_image, 
		VkDeviceMemory &_deviceMemory);
	// Transition Image Layout
	static void vkTransitionImageLayout(VKEngine* _engine,


			VkImage _image,
			VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout);

	// Command buffer recorder helper begin
	VkCommandBuffer vkBeginSingleTimeCommands();
	// Command buffer recorder helper end
	void vkEndSingleTimeCommands(VkCommandBuffer _commandBuffer);


	// Getter Functions
	// Get Vulkan Device
	VkDevice* vkGetDevice() { return m_vkDevice; }
	// Get Physical Device.
	VkPhysicalDevice* vkGetPhysicalDevice() { return m_vkPhysicalDevice; }
	// Get Graphics Queue
	VkQueue* vkGetGraphicsQueue() { return m_vkGraphicsQueue; }
	// Get Present Queue
	VkQueue* vkGetPresentQueue() { return m_vkPresentQueue; }
	// Get Swap chain
	VkSwapchainKHR* vkGetSwapChain() { return m_vkSwapChain; }
	// Get Swap Chain Format
	VkFormat vkGetSwapChainImageFormat() { return m_vkSwapChainImageFormat; }
	// Get Extent
	VkExtent2D vkGetSwapChainExtent() { return m_vkSwapChainExtent; }
	// Get Command Pool
	VkCommandPool* vkGetCommandPool() { return m_vkCommandPool; }
	// Get Command Buffers
	std::vector<VkCommandBuffer> vkGetCommandBuffers() { return m_vkCommandBuffers; }
	// Get swap chain images
	std::vector<VkImage> vkGetSwapChainImages() { return m_vkSwapChainImages; }
};

#endif