#include <optional>
#include <set>

#include "Engine.h"
#include "Controller.h"

// Debug based functions are also based out of the class for simplicity.
#pragma region DEBUG CALLBACKS
// Debug callbacks
#if GL
void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	const char* t_severity;
	if (severity == GL_DEBUG_SEVERITY_HIGH)
		t_severity = "HIGH";
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
		t_severity = "MEDIUM";
	else if (severity == GL_DEBUG_SEVERITY_LOW)
		t_severity = "LOW";
	else
		t_severity = "";

	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = %s, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, t_severity, message);
}

#elif VK
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

#endif

#pragma endregion End of Debug Callbacks

// To keep the engine class as neat as possible, vulkan based functions
//  are placed out of the class.
#pragma region VULKAN based functions
// Engine.cpp Vulkan functions
#if VK

const std::vector<const char*> vkDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct vkQueueFamilyIndices
{
	std::optional<uint32_t> m_vkGraphicsFamily;
	std::optional<uint32_t> m_vkPresentFamily;

	bool isComplete()
	{
		return m_vkGraphicsFamily.has_value() && m_vkPresentFamily.has_value();
	}
};

struct vkSwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR m_vkCapabilities;
	std::vector<VkSurfaceFormatKHR> m_vkFormats;
	std::vector<VkPresentModeKHR> m_vkPresentModes;
};

vkSwapChainSupportDetails vkQuerySwapChainSupport(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface)
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

vkQueueFamilyIndices vkFindQueueFamilies(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface)
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

VkSurfaceFormatKHR vkChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats)
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

VkPresentModeKHR vkChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _availablePresentModes) 
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

VkExtent2D vkChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities)
{
	if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
	{
		return _capabilities.currentExtent;
	}
	else 
	{
		// Choose window resolution
		VkExtent2D actualExtent = { WINDOW_WIDTH, WINDOW_HEIGHT};

		actualExtent.width = std::max(_capabilities.minImageExtent.width, std::min(_capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(_capabilities.minImageExtent.height, std::min(_capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void vkCreateSwapChain(VkDevice* _vkDevice, VkPhysicalDevice* _vkPhysicalDevice, VkSurfaceKHR* _vkSurface, VkSwapchainKHR* _vkSwapChain)
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
}

std::vector<const char*> vkGetRequiredExtensions()
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

void vkPopulateDebugMessengerCreateInfo
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

void vkSetupDebugMessenger(VkInstance* _vkInstance, VkDebugUtilsMessengerEXT* _vkMessenger)
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


bool vkCheckDeviceExtensionSupport(VkPhysicalDevice _vkDevice) 
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

bool vkIsDeviceSuitable(VkPhysicalDevice _vkDevice, VkSurfaceKHR* _vkSurface)
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
	// Returns true if queue families are gained, device extensions
	//  are supported, and if the swap chain is adequate
	return indices.isComplete() && ExtensionsSupported && swapChainAdequate;
}

void vkPickPhysicalDevice(VkInstance* _vkInstance, VkPhysicalDevice* _vkPhysicalDevice, VkSurfaceKHR* _vkSurface)
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

void vkCreateLogicalDevice(VkPhysicalDevice* _vkPhysicalDevice, VkDevice* _vkDevice, 
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
}

void vkCreateSurface(VkInstance* _vkInsance, GLFWwindow* _window, VkSurfaceKHR* _vkSurface)
{

	int err = glfwCreateWindowSurface(*_vkInsance, _window, nullptr, _vkSurface);
	if (err != VK_SUCCESS)
	{
		

		throw std::runtime_error("Failed to create window surface!");
	}

}

void vkCreateImageViews(VkDevice* _vkDevice,
	std::vector<VkImageView>& _vkSwapChainImageViews, 
	std::vector<VkImage>& _vkSwapChainImages,
	VkFormat* _vkSwapChainImageFormat )
{
	// Resize views to hold images
	_vkSwapChainImageViews.resize(_vkSwapChainImages.size());
	// Loop to iterate over the swap chain images
	for (size_t i = 0; i < _vkSwapChainImages.size(); i++)
	{
		// Create image view
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _vkSwapChainImages[i];
		// Specify view type
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = *_vkSwapChainImageFormat;
		// Specify components
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		// Specify sub resource range
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		// Create view
		if (vkCreateImageView(*_vkDevice, &createInfo, nullptr, &_vkSwapChainImageViews[i]) 
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}

#endif
#pragma endregion End of Vulkan based code.


Engine::Engine()
{
	// Create Window First before everything else
	m_myWindow = new Window();

#if VK
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

	if (vkCreateInstance(&createInfo, nullptr, m_vkInstance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance!");
	}

#endif

#ifdef DEBUG
#if GL
	// Enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

#elif VK
	// Enable debug output
	vkSetupDebugMessenger(m_vkInstance, m_vkDebugMessenger);
#endif
#endif // DEBUG

#if VK
	// Create Surface
	vkCreateSurface(m_vkInstance, m_myWindow->GetWindowComponent(), m_vkSurface);
	// Find Physical Device
	vkPickPhysicalDevice(m_vkInstance, m_vkPhysicalDevice, m_vkSurface);
	// Create Logical Device
	vkCreateLogicalDevice(m_vkPhysicalDevice, m_vkDevice, m_vkGraphicsQueue, m_vkSurface);
	// Create swapchain
	vkCreateSwapChain(m_vkDevice, m_vkPhysicalDevice, m_vkSurface, m_vkSwapChain);
#endif

	// Create Controller
	m_myController = new Controller();
	// Create Camera
	m_myCamera = new Camera();
	// Create Draw Engine
	m_myDrawEngine = new DrawEngine();
	// Create an object
	// Create and compile our GLSL program from the shaders
	m_object = new Object(*m_vkDevice, "Resources/Models/Sphere.obj"
	//,TextureType::BMP, "Resources/Textures/Penguin.bmp"
	//,"Resources/Textures/BrickHeightMap.bmp"
	);
	// Model matrix : an identity matrix (model will be at the origin)
	m_object->SetModelMatrix(glm::mat4(1.0f));
	
}

Engine::~Engine()
{
#if VK
	for (auto imageView : m_vkSwapChainImageViews) {
		vkDestroyImageView(*m_vkDevice, imageView, nullptr);
	}

	vkDestroySwapchainKHR(*m_vkDevice, *m_vkSwapChain, nullptr);
	vkDestroyDevice(*m_vkDevice, nullptr);

	if (EnableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(*m_vkInstance, *m_vkDebugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(*m_vkInstance, *m_vkSurface, nullptr);
	vkDestroyInstance(*m_vkInstance, nullptr);

	glfwTerminate();
#endif
}

void Engine::MainLoop()
{
	// Get deltatime
	glfwSetTime(0.0);
	double LastTime = glfwGetTime();
	float FrameRate = 0.0f;
	float FrameRefreshTime = 0.5f;
	float LastFPSUpdate = 0.0f; 

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
#if GL
			sprintf_s(x, 100, "OpenGL 4.6 - RayTracing Comparison - %3.2f ms/frame", double(m_deltaTime) * 1000);
#elif VK
			sprintf_s(x, 100, "Vulkan 1.1 - RayTracing Comparison - %3.2f ms/frame", double(m_deltaTime) * 1000);
#endif

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

		// Drawn objects
		m_myDrawEngine->Update(m_myCamera, m_myWindow, m_object);

		// record new last time
		LastTime = currentTime;

	} // Check if the ESC key was pressed or the window was closed
	while (m_myWindow->CheckWindowClose(m_myController) == 0);

}

