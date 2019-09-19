#if VK

#include <optional>
#include <set>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "VKEngine.h"
#include "VKQueueFamilyIndices.h"
#include "Controller.h"
#include "Pipeline.h"

#pragma region Debug Callbacks

#ifdef DEBUG
const bool EnableValidationLayers = true;
#else
const bool EnableValidationLayers = false;
#endif
// Validations layers
const std::vector<const char*> vkValidationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

// Create debug messenger
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
		"vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback
(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

#pragma endregion

VKEngine::VKEngine()
{
	// Create Window First before everything else
	m_myWindow = new Window();

	// Initialise Inherited Engine Code.
	Initialise();

	// Create Controller
	m_myController = new Controller();
	// Create Draw Engine
	// m_myDrawEngine = new VKDrawEngine(this);
}

VKEngine::~VKEngine()
{
	// Destroy command pool
	vkDestroyCommandPool(m_vkDevice, m_vkCommandPool, nullptr);
	// Destroy image views.
	for (auto imageView : m_vkSwapChainImageViews)
	{
		vkDestroyImageView(m_vkDevice, imageView, nullptr);
	}
	// Destroy Swap Chain.
	vkDestroySwapchainKHR(m_vkDevice, m_vkSwapChain, nullptr);
	// Destroy vulkan Device.
	vkDestroyDevice(m_vkDevice, nullptr);
	// If debug enabled, destroy debut utils Messenger.
	if (EnableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessenger, nullptr);
	}
	// Destroy Surface.
	vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
	// Destroy Instance.
	vkDestroyInstance(m_vkInstance, nullptr);

}


uint32_t VKEngine::vkFindMemoryType(int _typeFilter, VkMemoryPropertyFlags _vkProperties)
{
	// Physical device memory properties container.
	VkPhysicalDeviceMemoryProperties memProperties;
	// Get Reuquirements.
	vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice,
		&memProperties);
	//// Loop through memory types.
	//for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	//{
	//	std::cout << memProperties.memoryTypes[i].heapIndex << std::endl;
	//	if ((_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _vkProperties)
	//		== _vkProperties)
	//	{
	//		_typeFilter = i;
	//		return i;
	//	}
	//}

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((_typeFilter & 1) == 1)
		{
			if ((memProperties.memoryTypes[i].propertyFlags & _vkProperties) == _vkProperties)
			{
				_typeFilter = i;
				return i;
			}
		}
		_typeFilter >>= 1;
	}

	return -1;
}

void VKEngine::Initialise()
{
	// If vulkan, create vulkan instance
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Compute Ray Tracing";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "JBEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 54);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = vkGetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (EnableValidationLayers) 
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(vkValidationLayers.size());
		createInfo.ppEnabledLayerNames = vkValidationLayers.data();

		vkPopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else 
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	// Create vkInstance.
	if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create instance!");
	}

	// Init Debug
	vkSetupDebugMessenger(&m_vkInstance, &m_vkDebugMessenger);

	// Create Surface
	vkCreateSurface(&m_vkInstance, m_myWindow->GetWindowComponent(), &m_vkSurface);
	// Find Physical Device
	vkPickPhysicalDevice(&m_vkInstance, &m_vkPhysicalDevice, &m_vkSurface);
	// Create Logical Device
	vkCreateLogicalDevice(&m_vkPhysicalDevice, &m_vkDevice, &m_vkSurface);
	// Create swapchain
	vkCreateSwapChain(&m_vkDevice, &m_vkPhysicalDevice, &m_vkSurface, &m_vkSwapChain);

	// Create Image View
	vkCreateImageViews();

	// Create Compute image
	vkCreateComputeImage(m_vkComputeImage, m_vkComputeImageView, m_vkComputeImageDeviceMemory);

	// Prepare storage buffers
	vkPrepareStorageBuffers();

	// Create descriptor pool
	vkSetupDescriptorPool();
	// Prepare compute descriptor sets for pipeline
	vkPrepareComputeForPipelineCreation();
	// Create compute pipline
	vkCreateComputePipeline();

	// Create compute command pool
	vkCreateComputeCommandPool();
	// Create Compute command buffer
	vkCreateComputeCommandBuffer();
	// Record Compute command buffer
	vkRecordComputeCommandBuffer();
	// Create Compute Fence
	vkCreateComputeFence();

	// Create Semaphores
	vkCreateSemaphores();
}

void VKEngine::MainLoop()
{
	// Update
	vkUpdateUniformBuffer();
	// Draw
	vkDraw();
}


#pragma region Vulkan Setup Functions

void VKEngine::vkPopulateDebugMessengerCreateInfo
(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

void VKEngine::vkSetupDebugMessenger(VkInstance* _vkInstance, VkDebugUtilsMessengerEXT* _vkMessenger)
{
	// Return if validation layers are not enabled
	if (!EnableValidationLayers)
		return;

	// Create the debug utils messenger create info
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	vkPopulateDebugMessengerCreateInfo(createInfo);

	// Create the messenger
	if (CreateDebugUtilsMessengerEXT(*_vkInstance, &createInfo, nullptr, _vkMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

vkSwapChainSupportDetails VKEngine::vkQuerySwapChainSupport(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface)
{
	vkSwapChainSupportDetails details;
	// Get surface capabilities.
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_vkDevice, *_vkSurface, &details.m_vkCapabilities);

	// Get Format Count
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_vkDevice, *_vkSurface, &formatCount, nullptr);
	// Get surface formats
	if (formatCount != 0)
	{
		details.m_vkFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_vkDevice, *_vkSurface, &formatCount, details.m_vkFormats.data());
	}

	// Get Present Mode Count
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(_vkDevice, *_vkSurface, &presentModeCount, nullptr);
	// Resize vector to hold available formats
	if (presentModeCount != 0)
	{
		details.m_vkPresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_vkDevice, *_vkSurface, &presentModeCount, details.m_vkPresentModes.data());
	}

	return details;
}

//
//vkQueueFamilyIndices VKEngine::vkFindQueueFamilies(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface)
//{
//	// Indices to track
//	vkQueueFamilyIndices indices;
//	// Get Queue Family count
//	uint32_t queueFamilyCount = 0;
//	vkGetPhysicalDeviceQueueFamilyProperties(_vkDevice, &queueFamilyCount, nullptr);
//	// Get Queue Families
//	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//	vkGetPhysicalDeviceQueueFamilyProperties(_vkDevice, &queueFamilyCount, queueFamilies.data());
//
//	// Find Queue Family which supports VK_QUEUE_GRAPHICS_BIT
//	int i = 0;
//	for (const auto& queueFamily : queueFamilies)
//	{
//		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
//		{
//			indices.m_vkGraphicsFamily = i;
//		}
//
//		VkBool32 presentSupport = false;
//		vkGetPhysicalDeviceSurfaceSupportKHR(_vkDevice, i, *_vkSurface, &presentSupport);
//
//		if (queueFamily.queueCount > 0 && presentSupport)
//		{
//			indices.m_vkPresentFamily = i;
//		}
//
//		if (indices.isComplete())
//		{
//			break;
//		}
//
//		i++;
//	}
//	// Return QueueFamilies
//	return indices;
//}

VkSurfaceFormatKHR VKEngine::vkChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats)
{
	// Every format in VkSurfaceFormatKHR includes a format and a colorSpace member.
	//  The format member contains information on color channels and types, and the 
	//  colorSpace member says if the format is supported or not.
	for (const auto& availableFormat : _availableFormats)
	{
		// we want VK_FORMAT_B8G8R8A8_UNORM for the format and VK_FORMAT_B8G8R8A8_UNORM for the colorSpace
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_FORMAT_B8G8R8A8_UNORM)
		{
			return availableFormat;
		}
	}
	// If not, return the first format in the list.
	return _availableFormats[0];
}

VkPresentModeKHR VKEngine::vkChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes)
{
	// Want the best swap present mode. VK_PRESENT_MODE_FIFO_KHR is always availble, but 
	//  if VK_PRESENT_MODE_MAILBOX_KHR is available, pick that. A third option may be picked but not
	//  a priority:
	// Priotity 1:  VK_PRESENT_MODE_MAILBOX_KHR
	// Priotity 2:  VK_PRESENT_MODE_IMMEDIATE_KHR
	// Last Resort: VK_PRESENT_MODE_FIFO_KHR
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : _availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D VKEngine::vkChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities)
{
	if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return _capabilities.currentExtent;
	}
	else
	{
		// Choose window resolution
		VkExtent2D actualExtent = { WINDOW_WIDTH, WINDOW_HEIGHT };

		actualExtent.width = std::max(_capabilities.minImageExtent.width, std::min(_capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(_capabilities.minImageExtent.height, std::min(_capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void VKEngine::vkCreateSwapChain(VkDevice* _vkDevice, VkPhysicalDevice* _vkPhysicalDevice, VkSurfaceKHR* _vkSurface, VkSwapchainKHR* _vkSwapChain)
{
	
	auto swapChainSupport = vkQuerySwapChainSupport(m_vkPhysicalDevice, &m_vkSurface);
	auto surfaceFormat = vkChooseSwapSurfaceFormat(swapChainSupport.m_vkFormats);
	auto presentMode = vkChooseSwapPresentMode(swapChainSupport.m_vkPresentModes);
	auto extent = vkChooseSwapExtent(swapChainSupport.m_vkCapabilities);

	if (!(swapChainSupport.m_vkCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT))
		throw std::runtime_error("Swap chain doesn't support VK_IMAGE_USAGE_STORAGE BIT !");


	// Decide how many images we can work with at the same time (1, double buffering or triple buffering)
	uint32_t imageCount = swapChainSupport.m_vkCapabilities.minImageCount + 1;
	if (swapChainSupport.m_vkCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.m_vkCapabilities.maxImageCount)
		imageCount = swapChainSupport.m_vkCapabilities.maxImageCount;
	VkSwapchainCreateInfoKHR swapchainInfo{};
	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.surface = m_vkSurface;
	swapchainInfo.minImageCount = imageCount;
	swapchainInfo.imageFormat = surfaceFormat.format;
	swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainInfo.imageExtent = extent;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	sPreRenderedFrames += imageCount;

	// The Transform capabilities of the swap chain define which transformations are supported
	// I.e. portrait, landscape presentation etc.
	swapchainInfo.preTransform = swapChainSupport.m_vkCapabilities.currentTransform;
	// Ignore image's alpha channel (if there's any)
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	// Set the presentation mode. Mailbox is preferred since it works just like immediate but instead
	// of blocking the application when the queue is full, it simply replaces images with new ones.
	swapchainInfo.presentMode = presentMode;
	// Only render images when they're visible.
	swapchainInfo.clipped = VK_TRUE;

	// Use the current swap chain for recycling if there is any.
	VkSwapchainKHR oldSwapChain = m_vkSwapChain;
	swapchainInfo.oldSwapchain = oldSwapChain;

	VkSwapchainKHR newSwapChain;
	auto result = vkCreateSwapchainKHR(m_vkDevice, &swapchainInfo, nullptr, &newSwapChain);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create swap chain !");

	m_vkSwapChain = newSwapChain;

	vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &imageCount, nullptr);
	m_vkSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &imageCount, m_vkSwapChainImages.data());

	// Save swap chain format
	m_vkSwapChainImageFormat = surfaceFormat.format;
	// Save swap chain extents
	m_vkSwapChainExtent = extent;
}

std::vector<const char*> VKEngine::vkGetRequiredExtensions()
{
	// Extension count
	uint32_t glfwExtensionCount = 0;
	// Extensions list
	const char** glfwExtensions;
	// Get required extensions for GLFW
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	// Store extensions
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (EnableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	// Return extensions
	return extensions;
}

bool VKEngine::vkCheckDeviceExtensionSupport(VkPhysicalDevice _vkDevice)
{
	// Get total extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(_vkDevice, nullptr, &extensionCount, nullptr);
	// Get device extensions
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(_vkDevice, nullptr, &extensionCount, availableExtensions.data());
	// Store required extensions in string set
	std::set<std::string> requiredExtensions(vkDeviceExtensions.begin(), vkDeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool VKEngine::vkIsDeviceSuitable(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface)
{
	// See if queue families can be gained
	QueueFamilyIndices indices = FindQueueFamilies(_vkDevice, *_vkSurface);
	if (indices.IsComplete())
	{
		if (EnableValidationLayers)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(_vkDevice, &properties);
			std::cout << "Found GPU: " << properties.deviceName << std::endl;
		}
	}
	// Check device extension support
	bool ExtensionsSupported = vkCheckDeviceExtensionSupport(_vkDevice);
	// Check swap chain support
	bool swapChainAdequate = false;
	if (ExtensionsSupported)
	{
		vkSwapChainSupportDetails swapChainSupport = vkQuerySwapChainSupport(_vkDevice, _vkSurface);
		swapChainAdequate = !swapChainSupport.m_vkFormats.empty() && !swapChainSupport.m_vkPresentModes.empty();
	}
	// Get device featues 
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(_vkDevice, &features);
	// Returns true if queue families are gained, device extensions
	//  are supported, and if the swap chain is adequate
	return indices.IsComplete() && ExtensionsSupported && swapChainAdequate && features.samplerAnisotropy;
}

void VKEngine::vkPickPhysicalDevice(VkInstance* _vkInstance, VkPhysicalDevice* _vkPhysicalDevice, VkSurfaceKHR* _vkSurface)
{
	uint32_t DeviceCount = 0;
	// Get Total Physical Devices
	vkEnumeratePhysicalDevices(*_vkInstance, &DeviceCount, nullptr);
	// Throw error if none found with Vulkan Support
	if (DeviceCount == 0)
	{
		throw std::runtime_error("Failed to find any GPUs!");
	}
	// Create Array of physical devices
	std::vector<VkPhysicalDevice> AllDevices(DeviceCount);
	// Fill Array
	vkEnumeratePhysicalDevices(*_vkInstance, &DeviceCount, AllDevices.data());
	// Find first suitable device
	for (const auto& device : AllDevices)
	{
		// Check if device is suitable
		if (vkIsDeviceSuitable(device, _vkSurface))
		{
			*_vkPhysicalDevice = device;
			break;
		}
	}
	// Throw error if no Device was found
	if (_vkPhysicalDevice == nullptr)
	{
		throw std::runtime_error("Failed to find any GPUs with Vulkan Support!");
	}
}

void VKEngine::vkCreateLogicalDevice(VkPhysicalDevice* _vkPhysicalDevice, VkDevice* _vkDevice,
	VkSurfaceKHR* _vkSurface)
{
	// Get queue families
	QueueFamilyIndices indices = FindQueueFamilies(*_vkPhysicalDevice, *_vkSurface);
	// Queue Create Infos
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { indices.presentFamily, indices.computeFamily };
	// Make queue infos
	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;
	// Enable device extentions
	createInfo.enabledExtensionCount = static_cast<uint32_t>(vkDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = vkDeviceExtensions.data();

	if (EnableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(vkValidationLayers.size());
		createInfo.ppEnabledLayerNames = vkValidationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(*_vkPhysicalDevice, &createInfo, nullptr, _vkDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(*_vkDevice, indices.presentFamily, 0, &m_vkComputeQueue);
	vkGetDeviceQueue(*_vkDevice, indices.computeFamily, 0, &m_vkPresentQueue);
}

void VKEngine::vkCreateImageViews()
{
	m_vkSwapChainImageViews.resize(m_vkSwapChainImages.size());
	for (uint32_t i = 0; i < m_vkSwapChainImages.size(); i++)
	{
		// Component mapping
		VkComponentMapping components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

		// Image view create info
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_vkSwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_vkSwapChainImageFormat;
		createInfo.components = components;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		// Create Image View
		auto imageViewCreated = vkCreateImageView(m_vkDevice, &createInfo, nullptr, &m_vkSwapChainImageViews[i]);
		if (imageViewCreated != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image views!");
		}
	}
}

void VKEngine::vkCreateSurface(VkInstance* _vkInsance, GLFWwindow* _window, VkSurfaceKHR* _vkSurface)
{

	int err = glfwCreateWindowSurface(*_vkInsance, _window, nullptr, _vkSurface);
	if (err != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}

}

void VKEngine::vkCreateComputeImage(VkImage &img, VkImageView &imgView, VkDeviceMemory &memory)
{
	// Query swap chain support
	auto swapChainSupport = vkQuerySwapChainSupport(m_vkPhysicalDevice, &m_vkSurface);
	// Choose swap surface format.
	auto surfaceFormat = vkChooseSwapSurfaceFormat(swapChainSupport.m_vkFormats);

	// Image create info
	VkImageCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.imageType = VK_IMAGE_TYPE_2D;
	info.format = surfaceFormat.format;
	info.extent = { WINDOW_WIDTH, WINDOW_WIDTH, 1 };
	info.mipLevels = 1;
	info.arrayLayers = 1;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	info.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	auto result = vkCreateImage(m_vkDevice, &info, nullptr, &img);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create compute image !");
	}

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(m_vkDevice, img, &memReqs);
	int memTypeIndex = vkFindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkMemoryAllocateInfo memInfo{};
	memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memInfo.allocationSize = memReqs.size;
	memInfo.memoryTypeIndex = memTypeIndex;

	// Allocate memory for the image.
	result = vkAllocateMemory(m_vkDevice, &memInfo, nullptr, &memory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate memory for compute image !");
	}
	// Bind image to the memory.
	result = vkBindImageMemory(m_vkDevice, img, memory, 0);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to bind memory to compute image !");
	}

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = img;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = surfaceFormat.format;
	viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	result = vkCreateImageView(m_vkDevice, &viewInfo, nullptr, &imgView);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create compute image view !");
	}
}

void VKEngine::vkCreateStorageBuffer(const void * data, VkDeviceSize & bufferSize, VkBuffer & buffer, VkBufferUsageFlags bufferUsageFlags, VkDeviceMemory & deviceMemory, uint32_t memTypeIndex)
{
	// VkResult
	VkResult result;
	// Buffer create info
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.usage = bufferUsageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.size = bufferSize;
	// Create buffer
	result = vkCreateBuffer(m_vkDevice, &bufferInfo, nullptr, &buffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create storage buffer !");
	}

	// Get memory requirements
	VkMemoryRequirements memoryReqs;
	vkGetBufferMemoryRequirements(m_vkDevice, buffer, &memoryReqs);

	//std::cout << memReqs.memoryTypeBits << std::endl;
	memTypeIndex = vkFindMemoryType(memoryReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	// Memory allocate info
	VkMemoryAllocateInfo memoryInfo{};
	memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryInfo.allocationSize = memoryReqs.size;
	memoryInfo.memoryTypeIndex = memTypeIndex;

	// Allocate memory
	result = vkAllocateMemory(m_vkDevice, &memoryInfo, nullptr, &deviceMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate device memory for storage buffer !");
	}

	// Copy memory
	vkCopyMemory(data, deviceMemory, bufferSize);

	// Bind buffer memory
	vkBindBufferMemory(m_vkDevice, buffer, deviceMemory, 0);
}

void VKEngine::vkSetupDescriptorPool()
{
	// Storage descriptor pool size
	VkDescriptorPoolSize storageSize{};
	storageSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	storageSize.descriptorCount = 3;
	// Buffer descriptor pool size
	VkDescriptorPoolSize bufferSize{};
	bufferSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bufferSize.descriptorCount = 2;
	// Uniform descriptor pool size
	VkDescriptorPoolSize uniformSize{};
	uniformSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformSize.descriptorCount = 1;

	// Pool sizes
	std::vector<VkDescriptorPoolSize> poolSizes = { storageSize , bufferSize, uniformSize };

	// Descriptor pool create info
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.maxSets = 1;
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();

	// Create descriptor pool
	auto result = vkCreateDescriptorPool(m_vkDevice, &poolInfo, nullptr, &m_vkComputeDescriptorPool);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create Compute Descriptor Pool !");
}

void VKEngine::vkPrepareComputeForPipelineCreation()
{
	// Compute binding
	VkDescriptorSetLayoutBinding computeBinding{};
	computeBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	computeBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	computeBinding.descriptorCount = 1;
	computeBinding.binding = 0;
	// Sphere binding
	VkDescriptorSetLayoutBinding sphereBinding{};
	sphereBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	sphereBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	sphereBinding.descriptorCount = 1;
	sphereBinding.binding = 1;
	// Plane binding
	VkDescriptorSetLayoutBinding planeBinding{};
	planeBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	planeBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	planeBinding.descriptorCount = 1;
	planeBinding.binding = 2;
	// Uniform binding
	VkDescriptorSetLayoutBinding uniformBinding{};
	uniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	uniformBinding.descriptorCount = 1;
	uniformBinding.binding = 3;

	// All bindings
	std::vector<VkDescriptorSetLayoutBinding> bindings{ computeBinding, sphereBinding, planeBinding, uniformBinding };

	// Layout info
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindings.size();
	layoutInfo.pBindings = bindings.data();

	// Create descriptor set layout
	auto result = vkCreateDescriptorSetLayout(m_vkDevice, &layoutInfo, nullptr, &m_vkComputeDescriptorSetLayout);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create Compute DescriptorSet Layout !");

	// Pipeline layout info
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_vkComputeDescriptorSetLayout;
	
	// Create pipeline layout
	result = vkCreatePipelineLayout(m_vkDevice, &pipelineLayoutInfo, nullptr, &m_vkComputePipelineLayout);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to create Compute Pipeline Layout !");

	// Descriptor sets allocate info
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_vkComputeDescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &m_vkComputeDescriptorSetLayout;;

	// Create compute descriptor sets
	m_vkComputeDescriptorSets.resize(1);
	result = vkAllocateDescriptorSets(m_vkDevice, &allocInfo, m_vkComputeDescriptorSets.data());
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate Compute Descriptor Sets from Compute Descriptor Pool !");

	// Bind resources to the descriptor sets
	// Compute image
	VkDescriptorImageInfo computeInfo{};
	computeInfo.imageView = m_vkComputeImageView;
	computeInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	// Sphere Info
	VkDescriptorBufferInfo sphereInfo{};
	sphereInfo.buffer = m_vkSpheresBuffer;
	sphereInfo.offset = 0;
	sphereInfo.range = VK_WHOLE_SIZE;
	// Plane Info
	VkDescriptorBufferInfo planeInfo{};
	planeInfo.buffer = m_vkPlanesBuffer;
	planeInfo.offset = 0;
	planeInfo.range = VK_WHOLE_SIZE;
	// Uniform Info
	VkDescriptorBufferInfo uniformInfo{};
	uniformInfo.buffer = m_vkUniformBuffer;
	uniformInfo.offset = 0;
	uniformInfo.range = VK_WHOLE_SIZE;

	// Compute write descriptor set create info.
	VkWriteDescriptorSet computeWrite{};
	computeWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	computeWrite.dstSet = m_vkComputeDescriptorSets[0];
	computeWrite.dstBinding = 0;
	computeWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	computeWrite.descriptorCount = 1;
	computeWrite.pImageInfo = &computeInfo;
	// Sphere write descriptor set create info.
	VkWriteDescriptorSet sphereWrite{};
	sphereWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	sphereWrite.dstSet = m_vkComputeDescriptorSets[0];
	sphereWrite.dstBinding = 1;
	sphereWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	sphereWrite.descriptorCount = 1;
	sphereWrite.pBufferInfo = &sphereInfo;
	// Plane write descriptor set create info.
	VkWriteDescriptorSet planeWrite{};
	planeWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	planeWrite.dstSet = m_vkComputeDescriptorSets[0];
	planeWrite.dstBinding = 2;
	planeWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	planeWrite.descriptorCount = 1;
	planeWrite.pBufferInfo = &planeInfo;
	// Uniform write descriptor set create info.
	VkWriteDescriptorSet uniformWrite{};
	uniformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	uniformWrite.dstSet = m_vkComputeDescriptorSets[0];
	uniformWrite.dstBinding = 3;
	uniformWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformWrite.descriptorCount = 1;
	uniformWrite.pBufferInfo = &uniformInfo;

	// Write setds
	std::vector<VkWriteDescriptorSet> writeSets = { computeWrite, sphereWrite, planeWrite, uniformWrite };
	vkUpdateDescriptorSets(m_vkDevice, writeSets.size(), writeSets.data(), 0, VK_NULL_HANDLE);
}

void VKEngine::vkCreateComputePipeline()
{
	auto computeShaderCode = Pipeline::ReadFile("Shaders/comp.spv");
	VkShaderModule computeShaderModule;

	// Shader module create info
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = computeShaderCode.size();
	createInfo.pCode = (uint32_t*)computeShaderCode.data();

	// Create compute shader module.
	auto shaderModCreated = vkCreateShaderModule(m_vkDevice, &createInfo, nullptr, &computeShaderModule);
	if (shaderModCreated != VK_SUCCESS)
		throw std::runtime_error("Failed to create shader module!");

	// Compute shader stage info
	VkPipelineShaderStageCreateInfo computeStageInfo{};
	computeStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computeStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computeStageInfo.module = computeShaderModule;
	// Entry point of the shader.
	computeStageInfo.pName = "main";

	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.stage = computeStageInfo;
	pipelineInfo.layout = m_vkComputePipelineLayout;

	// Create pipeline.
	auto result = vkCreateComputePipelines(m_vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_vkComputePipeline);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Compute Pipelines!");
	}
}

void VKEngine::vkCreateComputeCommandPool()
{
	// Find queue families
	auto queueFamilyIndices = FindQueueFamilies(m_vkPhysicalDevice, m_vkSurface);

	// Command pool create info
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.computeFamily;

	// Create command pool.
	auto result = vkCreateCommandPool(m_vkDevice, &poolInfo, nullptr, &m_vkComputeCommandPool);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Compute Command Pool!");
	}
}

void VKEngine::vkCreateComputeCommandBuffer()
{
	// Set current image index to 0
	m_vkCurrentImageIndex = 0;

	// Command buffer allocate info
	VkCommandBufferAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = m_vkComputeCommandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = 1;

	// Allocate command buffer
	auto alloResult = vkAllocateCommandBuffers(m_vkDevice, &allocateInfo, &m_vkComputeCommandBuffer);
	if (alloResult != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Compute Command Buffers !");
	}
}

void VKEngine::vkRecordComputeCommandBuffer()
{
	// Command buffer begin info
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	// Begin command buffer
	auto result = vkBeginCommandBuffer(m_vkComputeCommandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Compute Command Buffer Recording couldn't be started !");
	}

	// Bind to pipeline
	vkCmdBindPipeline(m_vkComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_vkComputePipeline);

	// Bind descriptor set to current image
	vkCmdBindDescriptorSets(m_vkComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_vkComputePipelineLayout,
		0, 1, &m_vkComputeDescriptorSets[0], 0, 0);

	// Dispatch
	vkCmdDispatch(m_vkComputeCommandBuffer, m_vkSwapChainExtent.width, m_vkSwapChainExtent.height, 1);

	// Set first image memory barrier for each image
	vkSetFirstImageBarriers(m_vkComputeCommandBuffer, m_vkCurrentImageIndex);
	// Copy image to memory
	vkCopyImageMemory(m_vkComputeCommandBuffer, m_vkCurrentImageIndex);
	// Set second image memory barrier for each image
	vkSetSecondImageBarriers(m_vkComputeCommandBuffer, m_vkCurrentImageIndex);
	// End command buffer
	result = vkEndCommandBuffer(m_vkComputeCommandBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Compute Command Buffer Recording couldn't be ended !");
	}
}

void VKEngine::vkCreateComputeFence()
{
	// Fence create info
	VkFenceCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	// Create fence
	vkCreateFence(m_vkDevice, &info, nullptr, &m_vkComputeFence);
}

void VKEngine::vkCreateSemaphores()
{
	// Semaphore create info
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	// Image available semaphore
	auto availableResult = vkCreateSemaphore(m_vkDevice, &semaphoreInfo, nullptr, &m_vkImageAvailableSemaphore);
	// Image finished semaphore
	auto renderfinishResult = vkCreateSemaphore(m_vkDevice, &semaphoreInfo, nullptr, &m_vkRenderFinishedSemaphore);
	if (availableResult != VK_SUCCESS || renderfinishResult != VK_SUCCESS)
		throw std::runtime_error("Failed to create semaphores !");
}

void VKEngine::vkCopyMemory(const void * data, VkDeviceMemory & deviceMemory, VkDeviceSize & bufferSize)
{
	// map memory to the range of the data
	void* mapped = nullptr;
	auto result = vkMapMemory(m_vkDevice, deviceMemory, 0, VK_WHOLE_SIZE, 0, &mapped);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to map memory for storage buffer !");

	// copy the data to the mapped region
	std::memcpy(mapped, data, bufferSize);

	// memory is copied, mapped region is no longer needed.
	if (mapped)
	{
		vkUnmapMemory(m_vkDevice, deviceMemory);
		mapped = nullptr;
	}
}

void VKEngine::vkSetFirstImageBarriers(const VkCommandBuffer buffer, int curImageIndex)
{
	// Compute write memory barrier
	VkImageMemoryBarrier compWrite{};
	compWrite.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	compWrite.image = m_vkComputeImage;
	compWrite.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	compWrite.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	compWrite.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	compWrite.srcAccessMask = 0;
	compWrite.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

	// Compute transfer memory barrier
	VkImageMemoryBarrier compTransfer{};
	compTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	compTransfer.image = m_vkComputeImage;
	compTransfer.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	compTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	compTransfer.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	compTransfer.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	compTransfer.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

	// Swap memory barrier
	VkImageMemoryBarrier swapTransfer{};
	swapTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	swapTransfer.image = m_vkSwapChainImages[m_vkCurrentImageIndex];
	swapTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	swapTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	swapTransfer.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	swapTransfer.srcAccessMask = 0;
	swapTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	// Image memory barrier to make sure that compute shader writes are finished before sampling from the texture
	//VkImageMemoryBarrier imageMemoryBarrier = {};
	//imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	//imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	//imageMemoryBarrier.image = m_vkComputeImage;
	//imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	//imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	//imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	
	// The Three barriers
	m_vkBarriers.push_back(compWrite);
	m_vkBarriers.push_back(compTransfer);
	m_vkBarriers.push_back(swapTransfer);
	// std::vector<VkImageMemoryBarrier> barriers{ compWrite, compTransfer, swapTransfer };

	// Set pipeline barrier
	vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		0, 0, nullptr, 0, nullptr, m_vkBarriers.size(), m_vkBarriers.data());
}

void VKEngine::vkSetSecondImageBarriers(const VkCommandBuffer buffer, int curImageIndex)
{
	// Swap present
	VkImageMemoryBarrier swapPres{};
	swapPres.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	swapPres.image = m_vkSwapChainImages[curImageIndex];
	swapPres.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	swapPres.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	swapPres.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	swapPres.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	swapPres.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

	// Copy image barrier
	vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0, 0, nullptr, 0, nullptr, 1, &swapPres);
}

void VKEngine::vkCopyImageMemory(const VkCommandBuffer buffer, int curImageIndex)
{
	// Image source info
	VkImageSubresourceLayers source;
	source.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	source.mipLevel = 0;
	source.baseArrayLayer = 0;
	source.layerCount = 1;


	// Image destination info
	VkImageSubresourceLayers dest;
	dest.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	dest.mipLevel = 0;
	dest.baseArrayLayer = 0;
	dest.layerCount = 1;

	// Image copy info
	VkImageCopy copy;
	copy.srcSubresource = source;
	copy.dstSubresource = dest;
	copy.extent = { (uint32_t)Window::s_windowWidth, (uint32_t)Window::s_windowHeight, 1 };
	copy.srcOffset = { 0, 0, 0 };
	copy.dstOffset = { 0, 0, 0 };

	// Copy image to destination
	vkCmdCopyImage(buffer, m_vkComputeImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
		m_vkSwapChainImages[curImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
}

void VKEngine::vkUpdateUniformBuffer()
{
	// Set time
	m_vkUniformBufferObject.time = m_totalTime;
	// Get ubo size
	VkDeviceSize size = sizeof(m_vkUniformBufferObject);
	// Copy ubo to memory
	vkCopyMemory(&m_vkUniformBufferObject, m_vkUniformDeviceMemory, size);
}

void VKEngine::vkDraw()
{
	// Aquire next image
	vkAcquireNextImageKHR(m_vkDevice, m_vkSwapChain, std::numeric_limits<uint64_t>::max(), m_vkImageAvailableSemaphore, VK_NULL_HANDLE, &m_vkCurrentImageIndex);

	// Wait semaphores
	VkSemaphore waitSemaphores[] = { m_vkImageAvailableSemaphore };
	// Render finished semaphores
	VkSemaphore signalSemaphores[] = { m_vkRenderFinishedSemaphore };
	// Wait stages
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	// Compute submit info
	VkSubmitInfo computeSubmitInfo = {};
	computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	computeSubmitInfo.commandBufferCount = 1;
	computeSubmitInfo.pCommandBuffers = &m_vkComputeCommandBuffer;
	computeSubmitInfo.waitSemaphoreCount = 1;
	computeSubmitInfo.pWaitSemaphores = waitSemaphores;
	computeSubmitInfo.signalSemaphoreCount = 1;
	computeSubmitInfo.pSignalSemaphores = signalSemaphores;
	computeSubmitInfo.pWaitDstStageMask = waitStages;

	// Wait for fences
	vkWaitForFences(m_vkDevice, 1, &m_vkComputeFence, VK_TRUE, UINT64_MAX);
	// Reset fences once finished waiting
	vkResetFences(m_vkDevice, 1, &m_vkComputeFence);

	// Record compute command buffer
	vkRecordComputeCommandBuffer();

	// Submit queue
	auto resultSubmit = vkQueueSubmit(m_vkComputeQueue, 1, &computeSubmitInfo, m_vkComputeFence);
	if (resultSubmit != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit Compute Command Buffers to Compute Queue !");
	}

	// Present info
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	// Swap chain info
	VkSwapchainKHR swapChains[] = { m_vkSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &m_vkCurrentImageIndex;

	// Queue present
	auto result = vkQueuePresentKHR(m_vkPresentQueue, &presentInfo);

}



#pragma endregion
#endif