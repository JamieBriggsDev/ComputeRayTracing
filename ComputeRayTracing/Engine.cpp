#include "Engine.h"
#include "Controller.h"

// Disable nSight unknown object warning

Engine::Engine()
{
	// If vulkan, create vulkan instance
#if VK
		// initialize the VkApplicationInfo structure
	//VkApplicationInfo app_info = {};
	//app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	//app_info.pNext = NULL;
	//app_info.pApplicationName = "Compute Ray Tracing";
	//app_info.applicationVersion = 1;
	//app_info.pEngineName = "Compute Ray Tracing";
	//app_info.engineVersion = 1;
	//app_info.apiVersion = VK_API_VERSION_1_1;

	//// initialize the VkInstanceCreateInfo structure
	//VkInstanceCreateInfo inst_info = {};
	//inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	//inst_info.pNext = NULL;
	//inst_info.flags = 0;
	//inst_info.pApplicationInfo = &app_info;
	//inst_info.enabledExtensionCount = 0;
	//inst_info.ppEnabledExtensionNames = NULL;
	//inst_info.enabledLayerCount = 0;
	//inst_info.ppEnabledLayerNames = NULL;

	//VkResult res;

	//res = vkCreateInstance(&inst_info, NULL, m_vkInstance);
	//if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
	//	std::cout << "cannot find a compatible Vulkan ICD\n";
	//	exit(-1);
	//}
	//else if (res) {
	//	std::cout << "unknown error\n";
	//	exit(-1);
	//}
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::cout << extensionCount << " extensions supported" << std::endl;

	glm::mat4 matrix;
	glm::vec4 vec;
	auto test = matrix * vec;

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
	//vkDestroyInstance(*m_vkInstance, NULL);
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
