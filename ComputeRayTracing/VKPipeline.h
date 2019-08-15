#if VK

#pragma once
#include "Pipeline.h"
#include "VKEngine.h"
#include "VKModel.h"

#include <vulkan/vulkan.h>

class VKPipeline : public Pipeline
{
private:
	// VKEngine Reference
	VKEngine* m_vkEngineRef;
	// Render Pass
	VkRenderPass* m_vkRenderPass;
	// Descriptor Layout
	VkDescriptorSetLayout* m_vkDescriptorSetLayout;
	// Descriptor Pool
	VkDescriptorPool* m_vkDescriptorPool;
	// Descriptor sets
	std::vector<VkDescriptorSet> m_vkDescriptorSets;
	// Pipeline Layout
	VkPipelineLayout* m_vkPipelineLayout;
	// Graphics Pipeline
	VkPipeline* m_vkPipeline;
	// Texture Sampler
	VkSampler* m_vkTextureSampler;

	// Creates a shader module
	VkShaderModule CreateShaderModule(VkDevice _vkDevice, 
		const std::vector<char>& _code);
	// Create Pipeline
	void CreatePipelineLayout(VkExtent2D _vkSwapChainExtent,
		const char* _vertexFilePath,
		const char * _fragmentFilePath);
	// Create the Render Pass
	void CreateRenderPass(VkFormat _vkSwapChainImageFormat);
	// Create Descriptor Sets.
	void CreateDescriptorSetLayout();
	// Create Descriptor Pools.
	void CreateDescriptorPools();
	// Create Descriptor Sets.
	void CreateDescriptorSets(VKObject * _vkObject);
	// Creates texture sampler.
	void CreateTextureSampler();
public:
	VKPipeline(VKEngine* _vkEngine,

		VkExtent2D _vkSwapChainExtent,

		VkFormat _vkSwapChainImageFormat,

		const char* _vertexFilePath,

		const char * _fragmentFilePath, VKObject * _object);
	VkRenderPass* vkGetRenderPass() { return m_vkRenderPass; }
	VkPipeline* vkGetPipeline() { return m_vkPipeline; }
	VkPipelineLayout* vkGetPipelineLayout() { return m_vkPipelineLayout; }
	std::vector<VkDescriptorSet> vkGetDescriptorSets() { return m_vkDescriptorSets; }
	~VKPipeline();
};

#endif