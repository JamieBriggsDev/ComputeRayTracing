#include "Engine.h"

#include <iostream>
#include <sstream>

//#include "Timer.h"
#include "Timer.h"

int Engine::sPreRenderedFrames = 1;

void Engine::InitialiseObjects(std::vector<Plane> &_planes, std::vector<Sphere> &_spheres)
{
	auto AddPlane = [&_planes](Plane* go, glm::vec3 color, int type)
	{
		go->mat = Material(color, type);
		_planes.push_back(*go);
	};

	auto AddSphere = [&_spheres](Sphere* go, glm::vec3 color, int type)
	{
		go->mat = Material(color, type);
		_spheres.push_back(*go);
	};

	auto sphere = new Sphere(glm::vec3(0.0, 0.0, -5.0), 1.2);
	auto sphere2 = new Sphere(glm::vec3(-1.2, 1.5, -3.5), 0.7);
	auto sphere3 = new Sphere(glm::vec3(0.5, 0.7, -1.8), 0.4);

	auto bottom = new Plane(glm::vec3(0, 1, 0), 2.5);
	auto back = new Plane(glm::vec3(0, 0, 1), 5.5);
	auto left = new Plane(glm::vec3(1, 0, 0), 2.75);
	auto right = new Plane(glm::vec3(-1, 0, 0), 2.75);
	auto ceiling = new Plane(glm::vec3(0, -1, 0), 3.0);
	auto front = new Plane(glm::vec3(0, 0, -1), 0.5);

	// DIFFUSE_TYPE
	// REFLECT_TYPE
	// REFRACT_TYPE
	int testNumber = 1;

	switch (testNumber)
	{
	case 1:
		// Test 1
		AddSphere(sphere, glm::vec3(0.78, 0.29, 0.64), DIFFUSE_TYPE);
		AddSphere(sphere2, glm::vec3(0.062, 0.917, 0.078), DIFFUSE_TYPE);
		AddSphere(sphere3, glm::vec3(0.6, 0.2, 0.4), DIFFUSE_TYPE);
		break;
	case 2:
		// Test 2
		AddSphere(sphere, glm::vec3(0.78, 0.29, 0.64), REFLECT_TYPE);
		AddSphere(sphere2, glm::vec3(0.062, 0.917, 0.078), REFLECT_TYPE);
		AddSphere(sphere3, glm::vec3(0.6, 0.2, 0.4), REFLECT_TYPE);
		break;
	case 3:
		// Test 3
		AddSphere(sphere, glm::vec3(0.78, 0.29, 0.64), REFRACT_TYPE);
		AddSphere(sphere2, glm::vec3(0.062, 0.917, 0.078), REFRACT_TYPE);
		AddSphere(sphere3, glm::vec3(0.6, 0.2, 0.4), REFRACT_TYPE);
		break;
	case 4:
		// Test 4
		AddSphere(sphere, glm::vec3(0.78, 0.29, 0.64), REFLECT_TYPE);
		AddSphere(sphere2, glm::vec3(0.062, 0.917, 0.078), DIFFUSE_TYPE);
		AddSphere(sphere3, glm::vec3(0.6, 0.2, 0.4), REFRACT_TYPE);
		break;
	default:
		// No spheres!!
		std::cout << "No Test Selected!" << std::endl;
	}




	AddPlane(bottom, glm::vec3(0.5, 0.5, 0.5), DIFFUSE_TYPE);
	AddPlane(left, glm::vec3(1, 0.0, 0.0), DIFFUSE_TYPE);
	AddPlane(right, glm::vec3(0.0, 0.0, 1.0), DIFFUSE_TYPE);
	AddPlane(ceiling, glm::vec3(0.5, 0.5, 0.5), DIFFUSE_TYPE);
	AddPlane(back, glm::vec3(0.4, 1.0, 0.4), DIFFUSE_TYPE);
	AddPlane(front, glm::vec3(1.0, 0.8, 0.0), DIFFUSE_TYPE);
}


Engine::Engine()
{
	// Initialise objects
	InitialiseObjects(m_planes, m_spheres);
}

Engine::~Engine()
{
	// Terminate GLFW.
	glfwTerminate();
}

void Engine::Update()
{
	// Get deltatime
	glfwSetTime(0.0);
	double LastTime = glfwGetTime();
	float FrameRate = 0.0f;
	float FrameRefreshTime = 0.5f;
	float LastFPSUpdate = 0.0f;

	long long int TotalFrames = 0;
	do {
		// Record time for tests

		Timer::Instance()->TimeFrame();
		
		// End of test recording

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
			char* x = new char[100];
			// Get ms/frame.
#if GL
			sprintf_s(x, 100, "OpenGL 4.6");
#elif VK
			sprintf_s(x, 100, "Vulkan 1.1");
#endif
			// Get FPS.
			std::ostringstream oss;
			oss << x ;
			std::string var = oss.str();
			// Set the window title.
			glfwSetWindowTitle(m_myWindow->GetWindowComponent(), var.c_str());


			// Reset time variables
			//glfwSetTime(0.0);
			LastFPSUpdate = currentTime;
			TotalFrames = 0;
		}

		// Update controller
		m_myController->Update(m_myWindow, m_deltaTime);

		m_spheres.at(1).position.y = cos(m_totalTime);
		m_spheres.at(2).position.x = sin(m_totalTime);
		m_spheres.at(2).position.z = cos(m_totalTime) - 2.5f;
		//m_spheres.at(2).radius = sin(m_totalTime) / 2.0f;
		// Update the camera
		//m_myCamera->Update(m_myWindow, m_myController, m_deltaTime);


		// Engine dependent main loop
		MainLoop();

		// Update Window
		m_myWindow->Update();

		//
		m_totalTime += m_deltaTime;


		// record new last time
		LastTime = currentTime;


	} // Check if the ESC key was pressed or the window was closed
	while (m_myWindow->CheckWindowClose(m_myController) == 0 ||
		!Timer::Instance()->IsFinished());
}



