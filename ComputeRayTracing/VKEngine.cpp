#if VK

#include <optional>
#include <set>

#include "VKEngine.h"
#include "Controller.h"
#include "VKObject.h"
#include "VKPipeline.h"
#include "VKModel.h"
#include "VKTexture.h"

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
	// Create Camera
	m_myCamera = new Camera();
	// Create Draw Engine
	m_myDrawEngine = new VKDrawEngine(this);
}

VKEngine::~VKEngine()
{
	// Destroy command pool
	vkDestroyCommandPool(*m_vkDevice, *m_vkCommandPool, nullptr);
	// Destroy image views.
	for (auto imageView : m_vkSwapChainImageViews) 
	{
		vkDestroyImageView(*m_vkDevice, imageView, nullptr);
	}
	// Destroy Swap Chain.
	vkDestroySwapchainKHR(*m_vkDevice, *m_vkSwapChain, nullptr);
	// Destroy vulkan Device.
	vkDestroyDevice(*m_vkDevice, nullptr);
	// If debug enabled, destroy debut utils Messenger.
	if (EnableValidationLayers) 
	{
		DestroyDebugUtilsMessengerEXT(*m_vkInstance, *m_vkDebugMessenger, nullptr);
	}
	// Destroy Surface.
	vkDestroySurfaceKHR(*m_vkInstance, *m_vkSurface, nullptr);
	// Destroy Instance.
	vkDestroyInstance(*m_vkInstance, nullptr);

}

VkCommandBuffer VKEngine::vkBeginSingleTimeCommands()
{
	// Command buffer allocation info
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = *m_vkCommandPool;
	allocInfo.commandBufferCount = 1;
	// Command buffer to make
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(*m_vkDevice, &allocInfo, &commandBuffer);
	// Command buffer begin info
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	// Begin command buffer recording.
	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	// Return command buffer.
	return commandBuffer;
}

void VKEngine::vkEndSingleTimeCommands(VkCommandBuffer _commandBuffer)
{
	// End the command buffer
	vkEndCommandBuffer(_commandBuffer);
	// Command buffer submit info
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffer;
	// Submit to queue.
	vkQueueSubmit(*m_vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(*m_vkGraphicsQueue);
	// Free the command buffer.
	vkFreeCommandBuffers(*m_vkDevice, *m_vkCommandPool, 1, &_commandBuffer);
}

void VKEngine::Initialise()
{
	// If vulkan, create vulkan instance
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Compute Ray Tracing";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "JBEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = vkGetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (EnableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(vkValidationLayers.size());
		createInfo.ppEnabledLayerNames = vkValidationLayers.data();

		vkPopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	m_vkInstance = new VkInstance();
	m_vkPhysicalDevice = new VkPhysicalDevice();
	m_vkDevice = new VkDevice();
	m_vkDebugMessenger = new VkDebugUtilsMessengerEXT();
	m_vkGraphicsQueue = new VkQueue();
	m_vkPresentQueue = new VkQueue();
	m_vkSurface = new VkSurfaceKHR();
	m_vkSwapChain = new VkSwapchainKHR();

	// Create vkInstance.
	if (vkCreateInstance(&createInfo, nullptr, m_vkInstance) != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to create instance!");
	}

	// Init Debug
	vkSetupDebugMessenger(m_vkInstance, m_vkDebugMessenger);

	// Create Surface
	vkCreateSurface(m_vkInstance, m_myWindow->GetWindowComponent(), m_vkSurface);
	// Find Physical Device
	vkPickPhysicalDevice(m_vkInstance, m_vkPhysicalDevice, m_vkSurface);
	// Create Logical Device
	vkCreateLogicalDevice(m_vkPhysicalDevice, m_vkDevice, m_vkGraphicsQueue, m_vkSurface);
	// Create swapchain
	vkCreateSwapChain(m_vkDevice, m_vkPhysicalDevice, m_vkSurface, m_vkSwapChain);
	// Create image views
	vkCreateImageViews(m_vkDevice, m_vkSwapChainImageViews, m_vkSwapChainImages, &m_vkSwapChainImageFormat);
	// Create command pool.
	vkSetupCommandPool();

	// Create an object
	m_object = new VKObject(this,
		"Resources/Models/Sphere.obj");
	// Model matrix : an identity matrix (model will be at the origin)
	m_object->SetModelMatrix(glm::mat4(1.0f));
	// Setup depth resources
	vkSetupDepthBufferResources();
	// Create frame buffers
	vkCreateFrameBuffers();
	// Create command buffers.
	vkCreateCommandBuffers();

}

void VKEngine::MainLoop()
{
	// Get deltatime
	glfwSetTime(0.0);
	double LastTime = glfwGetTime();
	float FrameRate = 0.0f;
	float FrameRefreshTime = 0.5f;
	float LastFPSUpdate = 0.0f;

	//static auto startTime = std::chrono::high_resolution_clock::now();

	//auto currentTime = std::chrono::high_resolution_clock::now();
	//float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	long long int TotalFrames = 0;
	do {
		// Get delta time by comparing current time and last time
		double currentTime = glfwGetTime();
		m_deltaTime = float(currentTime - LastTime);
		if (currentTime - LastFPSUpdate < FrameRefreshTime)
		{
			// Increase TotalFrames
			TotalFrames++;
		}
		else
		{
			//nbFrames = 0;
			char* x = new char[100];

			sprintf_s(x, 100, "Vulkan 1.1 - RayTracing Comparison - %3.2f ms/frame", double(m_deltaTime) * 1000);


			FrameRate = (float)TotalFrames / (currentTime - LastFPSUpdate);
			char* fps = new char[10];
			sprintf_s(fps, 10, "%4.1f\n", FrameRate);

			std::ostringstream oss;
			oss << x << " (" << fps << " FPS)";
			std::string var = oss.str();

			glfwSetWindowTitle(m_myWindow->GetWindowComponent(), var.c_str());

			// Reset time variables
			//glfwSetTime(0.0);
			LastFPSUpdate = currentTime;
			TotalFrames = 0;
		}

		// Update controller
		m_myController->Update(m_myWindow, m_deltaTime);

		// Update the camera
		m_myCamera->Update(m_myWindow, m_myController, m_deltaTime);

		// Draw objects
		m_myDrawEngine->Update(static_cast<VKObject*>(m_object),
			m_vkCommandBuffers, m_deltaTime);

		// Update Window
		m_myWindow->Update();

		// record new last time
		LastTime = currentTime;

	} // Check if the ESC key was pressed or the window was closed
	while (m_myWindow->CheckWindowClose(m_myController) == 0);

	vkDeviceWaitIdle(*m_vkDevice);
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

vkQueueFamilyIndices VKEngine::vkFindQueueFamilies(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface)
{
	// Indices to track
	vkQueueFamilyIndices indices;
	// Get Queue Family count
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_vkDevice, &queueFamilyCount, nullptr);
	// Get Queue Families
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_vkDevice, &queueFamilyCount, queueFamilies.data());

	// Find Queue Family which supports VK_QUEUE_GRAPHICS_BIT
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.m_vkGraphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(_vkDevice, i, *_vkSurface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.m_vkPresentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}
	// Return QueueFamilies
	return indices;
}

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
	// Query the swap chain
	vkSwapChainSupportDetails swapChainSupport = vkQuerySwapChainSupport(*_vkPhysicalDevice, _vkSurface);
	// Get surface Format
	VkSurfaceFormatKHR surfaceFormat = vkChooseSwapSurfaceFormat(swapChainSupport.m_vkFormats);
	// Get Present Mode
	VkPresentModeKHR presentMode = vkChooseSwapPresentMode(swapChainSupport.m_vkPresentModes);
	// Get Swap Extent
	VkExtent2D extent = vkChooseSwapExtent(swapChainSupport.m_vkCapabilities);

	// Get Image count
	uint32_t imageCount = swapChainSupport.m_vkCapabilities.minImageCount + 1;
	// If can handle images, and can handle more than the swap chain support minimum
	if (swapChainSupport.m_vkCapabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.m_vkCapabilities.maxImageCount)
	{
		imageCount = swapChainSupport.m_vkCapabilities.maxImageCount;
	}

	// Swao chain creation
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = *_vkSurface;
	// Image properties
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Specify how to handle the swap chain images which are used across mulitple queue families
	vkQueueFamilyIndices indices = vkFindQueueFamilies(*_vkPhysicalDevice, _vkSurface);
	uint32_t queueFamilyIndices[] =
	{
		indices.m_vkGraphicsFamily.value(),
		indices.m_vkPresentFamily.value()
	};

	if (indices.m_vkGraphicsFamily != indices.m_vkPresentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	// Specify preTranform (if the images should be rotated. Dont want to so just
	//  specify current transform.)
	createInfo.preTransform = swapChainSupport.m_vkCapabilities.currentTransform;
	// Ignore alpha channel
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	// Specify present mode, set clipped to true (dont care about the color of pixels
	//  that are not visible due to another window. Set to true to enhance performance.)
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	// Dont want to use an old swap chain
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// Create the swap chain finally!!
	int err = vkCreateSwapchainKHR(*_vkDevice, &createInfo, nullptr, _vkSwapChain);
	if (err != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create the swap chain!");
	}

	// Get swapchain images
	vkGetSwapchainImagesKHR(*m_vkDevice, *_vkSwapChain, &imageCount, nullptr);
	m_vkSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(*m_vkDevice, *_vkSwapChain, &imageCount, m_vkSwapChainImages.data());

	m_vkSwapChainImageFormat = surfaceFormat.format;
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
	vkQueueFamilyIndices indices = vkFindQueueFamilies(_vkDevice, _vkSurface);
	if (indices.isComplete())
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
	return indices.isComplete() && ExtensionsSupported && swapChainAdequate && features.samplerAnisotropy;
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
	VkQueue* _vkGraphicsQueue, VkSurfaceKHR* _vkSurface)
{
	// Get queue families
	vkQueueFamilyIndices indices = vkFindQueueFamilies(*_vkPhysicalDevice, _vkSurface);
	// Queue Create Infos
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.m_vkGraphicsFamily.value(), indices.m_vkPresentFamily.value() };
	// Make queue infos
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
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

	vkGetDeviceQueue(*_vkDevice, indices.m_vkGraphicsFamily.value(), 0, _vkGraphicsQueue);
	vkGetDeviceQueue(*_vkDevice, indices.m_vkPresentFamily.value(), 0, m_vkPresentQueue);
}

void VKEngine::vkCreateSurface(VkInstance* _vkInsance, GLFWwindow* _window, VkSurfaceKHR* _vkSurface)
{

	int err = glfwCreateWindowSurface(*_vkInsance, _window, nullptr, _vkSurface);
	if (err != VK_SUCCESS)
	{


		throw std::runtime_error("Failed to create window surface!");
	}

}

void VKEngine::vkCreateImageViews(VkDevice* _vkDevice,
	std::vector<VkImageView>& _vkSwapChainImageViews,
	std::vector<VkImage>& _vkSwapChainImages,
	VkFormat* _vkSwapChainImageFormat)
{
	// Resize views to hold images
	_vkSwapChainImageViews.resize(_vkSwapChainImages.size());
	// Loop to iterate over the swap chain images
	for (size_t i = 0; i < _vkSwapChainImages.size(); i++)
	{
		_vkSwapChainImageViews[i] = VKTexture::CreateImageView(*m_vkDevice,
			_vkSwapChainImages[i],
			*_vkSwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		// TODO Remove!!!
		//// Create image view 
		//VkImageViewCreateInfo createInfo = {};
		//createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		//createInfo.image = _vkSwapChainImages[i];
		//// Specify view type
		//createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		//createInfo.format = *_vkSwapChainImageFormat;
		//// Specify components
		//createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		//createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		//createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		//createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		//// Specify sub resource range
		//createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		//createInfo.subresourceRange.baseMipLevel = 0;
		//createInfo.subresourceRange.levelCount = 1;
		//createInfo.subresourceRange.baseArrayLayer = 0;
		//createInfo.subresourceRange.layerCount = 1;
		//// Create view
		//if (vkCreateImageView(*_vkDevice, &createInfo, nullptr, &_vkSwapChainImageViews[i])
		//	!= VK_SUCCESS)
		//{
		//	throw std::runtime_error("failed to create image views!");
		//}
	}
}

uint32_t VKEngine::vkFindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _vkProperties, VKEngine* _vkEngine)
{
	// Physical device memory properties container.
	VkPhysicalDeviceMemoryProperties memProperties;
	// Get Reuquirements.
	vkGetPhysicalDeviceMemoryProperties(*_vkEngine->vkGetPhysicalDevice(),
		&memProperties);
	// Loop through memory types.
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
	{
		if ((_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _vkProperties)
			== _vkProperties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VKEngine::vkSetupImage(uint32_t _width, uint32_t _height, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties, VKEngine * _vkEngine, VkImage &_image, VkDeviceMemory &_deviceMemory)
{
	// Image create info
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = _width; 
	imageInfo.extent.height = _height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	// Image format
	imageInfo.format = _format;
	// Tiling mode
	imageInfo.tiling = _tiling;
	// Image layout
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Image usage
	imageInfo.usage = _usage;
	// Image sharing mode
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// Image samples
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	// flags
	imageInfo.flags = 0;

	// Create the image
	//_image = new VkImage();
	if (vkCreateImage(*_vkEngine->vkGetDevice(), &imageInfo, nullptr, &_image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}

	// Allocate texture image memory
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(*_vkEngine->vkGetDevice(),
		_image,
		&memRequirements);

	// Allocate info
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	// Find memory type
	allocInfo.memoryTypeIndex = VKEngine::vkFindMemoryType(memRequirements.memoryTypeBits,
		_properties, _vkEngine);
	// Allocate memory
	//_deviceMemory = new VkDeviceMemory();
	if (vkAllocateMemory(*_vkEngine->vkGetDevice(), &allocInfo, nullptr, &_deviceMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}
	// Bind image to memory.
	vkBindImageMemory(*_vkEngine->vkGetDevice(),
		_image,
		_deviceMemory, 0);
}

void VKEngine::vkTransitionImageLayout(VKEngine* _engine, VkImage _image, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout)
{
	// Start command buffer recording.
	VkCommandBuffer commandBuffer = _engine->vkBeginSingleTimeCommands();

	// Image memory barrier
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = _oldLayout;
	barrier.newLayout = _newLayout;
	// Ignore queue families
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	// Image params
	barrier.image = _image;

	// Check for depth stenco; attachment
	if (_newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (vkHasStencilComponent(_format))
		{
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else 
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}


	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	// No mipmapping
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;

	// Get source and destination pipeline stage
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("Unsupported layout transition!");
	}


	// Submit pipeline barrier
 	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage,
		destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	// End command buffer recording.
	_engine->vkEndSingleTimeCommands(commandBuffer);
}

void VKEngine::vkCreateFrameBuffers()
{
	// Resize swap chain frame buffers to be same size as swap chain image views.
	m_vkSwapChainFrameBuffers.resize(m_vkSwapChainImageViews.size());

	// Go through all swap chain image views
	for (size_t i = 0; i < m_vkSwapChainImageViews.size(); i++) 
	{
		std::array<VkImageView, 2> attachments = {
			m_vkSwapChainImageViews[i],
			*m_vkDepthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = *static_cast<VKPipeline*>(m_object->GetPipeline())->vkGetRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_vkSwapChainExtent.width;
		framebufferInfo.height = m_vkSwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(*m_vkDevice, &framebufferInfo, nullptr, &m_vkSwapChainFrameBuffers[i])
			!= VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create framebuffer!");
		}

	}
}

void VKEngine::vkSetupCommandPool()
{
	// Get queue families.
	vkQueueFamilyIndices queueFamilyIndices = vkFindQueueFamilies(*m_vkPhysicalDevice, m_vkSurface);
	// Command pool create info.
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.m_vkGraphicsFamily.value();
	poolInfo.flags = 0; // Optional

	// Create the command pool
	m_vkCommandPool = new VkCommandPool();
	if (vkCreateCommandPool(*m_vkDevice, &poolInfo, nullptr, m_vkCommandPool)
		!= VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool!");
	}
}

void VKEngine::vkCreateCommandBuffers()
{
	// Resize command buffers vector to be same size as frame buffers.
	m_vkCommandBuffers.resize(m_vkSwapChainImageViews.size());

	// Clear Colors
	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = { 0.92f, 0.6f, 0.6f, 0.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	// Command buffer create info
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = *m_vkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_vkCommandBuffers.size();

	// Create the command buffers
	if (vkAllocateCommandBuffers(*m_vkDevice, &allocInfo, m_vkCommandBuffers.data())
		!= VK_SUCCESS)
	{
		throw runtime_error("Failed to allocate command buffers!");
	}

	// Start recording command buffers
	// loop through command buffers.
	for (size_t i = 0; i < m_vkCommandBuffers.size(); i++)
	{
		// Command buffer begin create info
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional
		// Begin record
		if (vkBeginCommandBuffer(m_vkCommandBuffers[i], &beginInfo) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		// Begin Render Pass info
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = 
			*static_cast<VKPipeline*>(m_object->GetPipeline())->vkGetRenderPass();
		renderPassInfo.framebuffer = m_vkSwapChainFrameBuffers[i];
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_vkSwapChainExtent;
		// Clear Color of back screen.
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// Begin render pass
		vkCmdBeginRenderPass(m_vkCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			// Bind graphics pipeline
			vkCmdBindPipeline(m_vkCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
				*static_cast<VKPipeline*>(m_object->GetPipeline())->vkGetPipeline());

			VKModel* temp = static_cast<VKModel*>(m_object->GetModel());

			// Bind Vertex and Index buffers.
			VkBuffer vertexBuffers[] = { temp->vkGetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(m_vkCommandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(m_vkCommandBuffers[i], 
				temp->vkGetIndexBuffer(), 
				0, 
				VK_INDEX_TYPE_UINT32);

			VKPipeline* myPipeline = static_cast<VKPipeline*>(m_object->GetPipeline());
			// Bind Descriptor Sets
			vkCmdBindDescriptorSets(m_vkCommandBuffers[i], 
				VK_PIPELINE_BIND_POINT_GRAPHICS, 
				*myPipeline->vkGetPipelineLayout(),
				0, 1, 
				&myPipeline->vkGetDescriptorSets()[i],
				0, 
				nullptr);
			// Draw with indices.
			vkCmdDrawIndexed(m_vkCommandBuffers[i], static_cast<uint32_t>(temp->vkGetIndices().size()), 1, 0, 0, 0);
		
		// End render Pass
		vkCmdEndRenderPass(m_vkCommandBuffers[i]);
		// End Command buffer
		if (vkEndCommandBuffer(m_vkCommandBuffers[i])
			!= VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to record command buffer.");
		}
		
	}
}

void VKEngine::vkSetupDepthBufferResources()
{
	// Get depth format.
	VkFormat depthFormat = vkFindDepthFormat();
	// Create image.
	m_vkDepthImage = new VkImage();
	m_vkDepthImageMemory = new VkDeviceMemory();
	VKEngine::vkSetupImage(m_vkSwapChainExtent.width,
		m_vkSwapChainExtent.height,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		this,
		*m_vkDepthImage,
		*m_vkDepthImageMemory);

	// Create depth image view
	m_vkDepthImageView = new VkImageView();

	*m_vkDepthImageView = VKTexture::CreateImageView(*m_vkDevice, *m_vkDepthImage, 
		depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	vkTransitionImageLayout(this, *m_vkDepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

VkFormat VKEngine::vkFindDepthFormat()
{
	// Find supported format.
	return vkFindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool VKEngine::vkHasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat VKEngine::vkFindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	// Go through all candidates.
	for (VkFormat format : candidates) 
	{
		// Get physical device format properties.
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(*m_vkPhysicalDevice, format, &props);
		// Find suitable format
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}

	}
	// Error if no supported format.
	throw std::runtime_error("Failed to find supported format!");
}




#pragma endregion
#endif