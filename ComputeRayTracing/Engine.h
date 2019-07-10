#pragma once

#include "Window.h"
#include "Camera.h"
#include "DrawEngine.h"

#if VK
#include <vulkan/vulkan.h>
#endif

class Engine
{
private:
#if VK
	VkInstance* m_vkInstance;
#endif
	// Window object
	Window* m_myWindow;
	// Camera Object
	Camera* m_myCamera;
	// Controller Component
	Controller* m_myController;
	// Draw Engine
	DrawEngine* m_myDrawEngine;
	// Objects
	Object* m_object;
	// DeltaTime
	float m_deltaTime;
public:
	Engine();
	~Engine();
	void MainLoop();

};

