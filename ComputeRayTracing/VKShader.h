#if VK

#pragma once
#include "Shader.h"

#include <vulkan/vulkan.h>

class VKShader : public Shader
{
private:
	VkShaderModule CreateShaderModule(VkDevice _vkDevice, 
		const std::vector<char>& _code);
public:
	VKShader(VkDevice _vkDevice, const char* _vertexFilePath, const char * _fragmentFilePath);
	~VKShader();
};

#endif