#if VK

#pragma once
#include "Pipeline.h"

#include <vulkan/vulkan.h>

class VKPipeline : public Pipeline
{
private:
	VkShaderModule CreateShaderModule(VkDevice _vkDevice, 
		const std::vector<char>& _code);
	VkDevice* m_vkDevice;
	// Render Pass
	VkRenderPass* m_vkRenderPass;
	// Pipeline Layout
	VkPipelineLayout* m_vkPipelineLayout;
	// Graphics Pipeline
	VkPipeline* m_vkPipeline;

	// Create Pipeline
	void CreatePipelineLayout(VkExtent2D _vkSwapChainExtent,
		const char* _vertexFilePath,
		const char * _fragmentFilePath);
	// Create the Render Pass
	void CreateRenderPass(VkFormat _vkSwapChainImageFormat);
public:
	VKPipeline(VkDevice* _vkDevice,
		VkExtent2D _vkSwapChainExtent,
		VkFormat _vkSwapChainImageFormat,
		const char* _vertexFilePath,
		const char * _fragmentFilePath);
	~VKPipeline();
};

#endif