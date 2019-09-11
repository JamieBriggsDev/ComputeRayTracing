#pragma once
#if VK
// Vulkan
#include <vulkan/vulkan.h>
// Optional
#include <optional>

#include "Engine.h"
#include "VKDrawEngine.h"
#include "Shapes.h"



// Vulkan Extensions
const std::vector<const char*> vkDeviceExtensions = {
VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
	// VKDrawEngine* m_myDrawEngine;

	// Vulkan Instace
	VkInstance* m_vkInstance;
	// Vulkan Physical Device
	VkPhysicalDevice* m_vkPhysicalDevice;
	// Vulkan Logical Device
	VkDevice* m_vkDevice;
	// Debug Messenger
	VkDebugUtilsMessengerEXT* m_vkDebugMessenger;
	// Compute Queue
	VkQueue* m_vkComputeQueue;
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

	// Compute shader stuff
	// Compute shader output
	VkImage m_vkComputeImage;
	// Compute image view
	VkImageView m_vkComputeImageView;
	// Compute image device memory
	VkDeviceMemory m_vkComputeImageDeviceMemory;

	// Planes buffer
	VkBuffer m_vkPlanesBuffer;
	// Planes buffer device memory
	VkDeviceMemory m_vkPlanesDeviceMemory;
	// Sphere buffer
	VkBuffer m_vkSpheresBuffer;
	// Sphere buffer device memory
	VkDeviceMemory m_vkSphereDeviceMemory;
	// Uniform Buffer
	VkBuffer m_vkUniformBuffer;
	// Uniform buffer device memory
	VkDeviceMemory m_vkUniformDeviceMemory;

	// Compute descriptor pool
	VkDescriptorPool m_vkComputeDescriptorPool;
	// Compute descriptor set layout
	VkDescriptorSetLayout m_vkComputeDescriptorSetLayout;
	// Compute pipeline layout
	VkPipelineLayout m_vkComputePipelineLayout;
	// Compute descriptor sets
	std::vector<VkDescriptorSet> m_vkComputeDescriptorSets;
	// Compute pipeline
	VkPipeline m_vkComputePipeline;
	// Compute command pool
	VkCommandPool m_vkComputeCommandPool;

	// Image available semaphore
	VkSemaphore m_vkImageAvailableSemaphore;
	// Image finished rendering semaphore
	VkSemaphore m_vkRenderFinishedSemaphore;

	// Current compute image index
	uint32_t m_vkCurrentImageIndex;
	// Compute command buffder
	VkCommandBuffer m_vkComputeCommandBuffer;

	// Compute fence
	VkFence m_vkComputeFence;

	// Image barriers
	std::vector<VkImageMemoryBarrier> m_vkBarriers;
	

	// Uniform Buffer
	struct UBO
	{
		float time;
	} m_vkUniformBufferObject;

	VKObject* m_object;

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
	// Choose best suitable swap surface format.
	VkSurfaceFormatKHR vkChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats);
	// Chose best suitable swap present mode.
	VkPresentModeKHR vkChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes);
	// Choose best suitable swap extent.
	VkExtent2D vkChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities);
	// Create Swap Chain.
	void vkCreateSwapChain(VkDevice* _vkDevice, VkPhysicalDevice* _vkPhysicalDevice,
		VkSurfaceKHR* _vkSurface, VkSwapchainKHR* _vkSwapChain);

	// Get Required Extensions.
	std::vector<const char*> vkGetRequiredExtensions();
	// Check Device Extension Support.
	bool vkCheckDeviceExtensionSupport(VkPhysicalDevice _vkDevice);
	// Checks if Device is suitable.
	bool vkIsDeviceSuitable(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface);

	// Picks a physical Device.
	void vkPickPhysicalDevice(VkInstance* _vkInstance, VkPhysicalDevice* _vkPhysicalDevice, VkSurfaceKHR* _vkSurface);
	// Creates the Logical Device.
	void vkCreateLogicalDevice(VkPhysicalDevice* _vkPhysicalDevice, VkDevice* _vkDevice,
		VkSurfaceKHR* _vkSurface);

	// Create Image View
	void vkCreateImageViews();

	// Creates the surface.
	void vkCreateSurface(VkInstance* _vkInsance, GLFWwindow* _window, VkSurfaceKHR* _vkSurface);



	// Create compute image
	void vkCreateComputeImage(VkImage &img, VkImageView &imgView, VkDeviceMemory &memory);
	// Prepare Storage Buffers
	void vkPrepareStorageBuffers();

	// Create Storage Buffer
	void vkCreateStorageBuffer(const void* data, VkDeviceSize &bufferSize, VkBuffer &buffer,
		VkBufferUsageFlags bufferUsageFlags, VkDeviceMemory &deviceMemory, uint32_t memTypeIndex);

	// Create Descriptor pool
	void vkSetupDescriptorPool();
	// Prepare Compute for pipeline creation
	void vkPrepareComputeForPipelineCreation();
	// Create the compute pipeline
	void vkCreateComputePipeline();

	// Create Compute command pool
	void vkCreateComputeCommandPool();
	// Creates compute command buffer
	void vkCreateComputeCommandBuffer();
	// Records compute command buffer
	void vkRecordComputeCommandBuffer();
	// Creates compute fence
	void vkCreateComputeFence();
	// Create Semaphores
	void vkCreateSemaphores();

	// Copy data
	void vkCopyMemory(const void* data, VkDeviceMemory &deviceMemory, VkDeviceSize &bufferSize);

	// Set first image barrier
	void vkSetFirstImageBarriers(const VkCommandBuffer buffer, int curImageIndex);
	// Set second
	void vkSetSecondImageBarriers(const VkCommandBuffer buffer, int curImageIndex);
	// Copy image to memory
	void vkCopyImageMemory(const VkCommandBuffer buffer, int curImageIndex);

	// Update uniform buffer
	void vkUpdateUniformBuffer();
	// Draw
	void vkDraw();


public:
	VKEngine();
	~VKEngine();

	// Command buffer recorder helper begin
	VkCommandBuffer vkBeginSingleTimeCommands();
	// Command buffer recorder helper end
	void vkEndSingleTimeCommands(VkCommandBuffer _commandBuffer);

	// Finds the memory type for creating vertex bufers.
	uint32_t vkFindMemoryType(int _typeFilter,
		VkMemoryPropertyFlags _vkProperties);

	// Getter Functions
	// Get Vulkan Device
	VkDevice* vkGetDevice() { return m_vkDevice; }
	// Get Physical Device.
	VkPhysicalDevice* vkGetPhysicalDevice() { return m_vkPhysicalDevice; }
	// Get Graphics Queue
	VkQueue* vkGetComputeQueue() { return m_vkComputeQueue; }
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