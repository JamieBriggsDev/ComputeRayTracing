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
	// Descriptor Layout
	VkDescriptorSetLayout* m_vkDescriptorSetLayout;
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
	// Create Descriptor Sets.
	void CreateDescriptorSets();
public:
	VKPipeline(VkDevice* _vkDevice,
		VkExtent2D _vkSwapChainExtent,
		VkFormat _vkSwapChainImageFormat,
		const char* _vertexFilePath,
		const char * _fragmentFilePath);
	VkRenderPass* vkGetRenderPass() { return m_vkRenderPass; }
	VkPipeline* vkGetPipeline() { return m_vkPipeline; }

	~VKPipeline();
};

#endif