#if VK
#include "VKShader.h"

// Init Vulkan Shader Component
VKShader::VKShader(VkDevice _vkDevice, 
	const char* _vertexFilePath, 
	const char * _fragmentFilePath)
{
	// Get the shader code.
	std::vector<char> fragShaderCode = ReadFile(_fragmentFilePath);
	std::vector<char> vertShaderCode = ReadFile(_vertexFilePath);

	VkShaderModule fragShaderModule = CreateShaderModule(_vkDevice, fragShaderCode);
	VkShaderModule vertShaderModule = CreateShaderModule(_vkDevice, vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	vkDestroyShaderModule(_vkDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(_vkDevice, vertShaderModule, nullptr);
}

VKShader::~VKShader()
{
}

// Create Shader Module.
VkShaderModule VKShader::CreateShaderModule(VkDevice _vkDevice, const std::vector<char>& _code)
{
	// Shader module create info.
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = _code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());
	// Create shader module.
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(_vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module!");
	}
	// Return the shader module.
	return shaderModule;
}

#endif