#include "Engine.h"
#include "Controller.h"

// Disable nSight unknown object warning

Engine::Engine()
{
	// If vulkan, create vulkan instance
#if VK
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

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;
	m_vkInstance = new VkInstance();
	if (vkCreateInstance(&createInfo, nullptr, m_vkInstance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}

#endif
	// Create Window
	m_myWindow = new Window();
	// Create Controller
	m_myController = new Controller();
	// Create Camera
	m_myCamera = new Camera();
	// Create Draw Engine
	m_myDrawEngine = new DrawEngine();
	// Create an object
	// Create and compile our GLSL program from the shaders
	m_object = new Object("Resources/Models/Sphere.obj"
	//,TextureType::BMP, "Resources/Textures/Penguin.bmp"
	//,"Resources/Textures/BrickHeightMap.bmp"
	);
	// Model matrix : an identity matrix (model will be at the origin)
	m_object->SetModelMatrix(glm::mat4(1.0f));
}

Engine::~Engine()
{
#if VK
	// Destroy vkInstance
	vkDestroyInstance(*m_vkInstance, nullptr);
#endif
}

void Engine::MainLoop()
{
	// Get deltatime
	double lastTime = glfwGetTime();
	double lastTimeDT = lastTime;
	long long int nbFrames = 0;
	do {
		// Get delta time by comparing current time and last time
		double currentTime = glfwGetTime();
		nbFrames++;
		m_deltaTime = float(currentTime - lastTimeDT);
		if (currentTime - lastTime >= 1.0)
		{
			//nbFrames = 0;
			char* x = new char[100];
#if GL
			sprintf_s(x, 100, "OpenGL 4.6 - RayTracing Comparison - %3.2f ms/frame", double(m_deltaTime) * 1000);
#elif VK
			sprintf_s(x, 100, "Vulkan 1.1 - RayTracing Comparison - %3.2f ms/frame", double(m_deltaTime) * 1000);
#endif

			lastTime += 1.0;
			char* fps = new char[10];
			sprintf_s(fps, 10, "%4.1f\n", double(nbFrames) / (currentTime));
			//std::cout << "FPS: " << fps << std::endl;
			// Change title of window to show ms/frame time.
			// Create Title
			std::ostringstream oss;
			oss << x << " (" << fps << " FPS)";
			std::string var = oss.str();

			glfwSetWindowTitle(m_myWindow->GetWindowComponent(), var.c_str());
		}
		//lastTime = currentTime;


		// Update controller
		m_myController->Update(m_myWindow, m_deltaTime);

		// Update the camera
		m_myCamera->Update(m_myWindow, m_myController, m_deltaTime);

		// Drawn objects
		m_myDrawEngine->Update(m_myCamera, m_myWindow, m_object);

		// record new last time
		lastTimeDT = currentTime;

	} // Check if the ESC key was pressed or the window was closed
	while (m_myWindow->CheckWindowClose(m_myController) == 0);

}
