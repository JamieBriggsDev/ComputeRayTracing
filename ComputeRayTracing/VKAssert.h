#if VK
#pragma once
#include <vulkan/vulkan.h>
#include <iostream>

// Assert check
#define VK_CHECK_RESULT(f)																			
{																									
	VkResult res = (f);																				
	if (res != VK_SUCCESS)																																										
		throw std::runtime_error("Error has occured!");												
	
}


#endif
