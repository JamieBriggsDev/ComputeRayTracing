#pragma once
#if VK
// Vulkan
#include <vulkan/vulkan.h>
// Optional
#include <optional>

#include "Engine.h"




class VKEngine :
	public Engine
{
private:


	// Vulkan Engine Init Code;
	void Initialise();
	// Vulkan Engine Main Loop
	void MainLoop();

};

#endif