#include "Engine.h"
#include "Controller.h"

// Disable nSight unknown object warning

Engine::Engine()
{
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

	// light blue background
	glClearColor(0.6f, 0.85f, 0.92f, 0.0f);

	// Enable face culling
	glEnable(GL_CULL_FACE);



	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
}

Engine::~Engine()
{
}

void Engine::MainLoop()
{
	// Get deltatime
	double lastTime = glfwGetTime();
	do {
		// Get delta time by comparing current time and last time
		double currentTime = glfwGetTime();
		m_deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;
		//std::cout << "del: " << m_deltaTime << "\tCur: " << currentTime << "\tLas: " << lastTime << std::endl;
		//std::cout << currentTime << "   " << lastTime << std::endl;
		//if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
		 // printf and reset timer
			//printf("%f ms/frame\n", 1000.0 / double(totalFrames));
			char* x = new char[100];
			sprintf_s(x, 100, "RayTracing Comparison - %f ms/frame\n", double(m_deltaTime) * 1000);
			glfwSetWindowTitle(m_myWindow->GetWindowComponent(), x);
		//}

		// Update controller
		m_myController->Update(m_myWindow, m_deltaTime);

		// Update the camera
		m_myCamera->Update(m_myWindow, m_myController, m_deltaTime);

		//// Update Window
		//m_myWindow->Update();

		// Drawn objects
		m_myDrawEngine->Update(m_myCamera, m_myWindow, m_object);

		// record new last time
		lastTime = currentTime;

	} // Check if the ESC key was pressed or the window was closed
	while (m_myWindow->CheckWindowClose(m_myController) == 0);

}
