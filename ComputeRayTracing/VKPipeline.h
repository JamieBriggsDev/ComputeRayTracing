#if VK

#pragma once
#include "Pipeline.h"
#include "VKEngine.h"
#include "VKBuffer.h"
#include "Shapes.h"

#include <vulkan/vulkan.h>

class VKTexture;

struct storageBuffer
{
	// Buffer for spheres
	VKBuffer spheres;
	// Buffer for planes
	VKBuffer planes;
};

class VKPipeline : public Pipeline
{
private:
	// Total objects
	int m_totalObjects = 0;
	// VKEngine Reference
	VKEngine* m_vkEngineRef;
	// Compute render target
	VKTexture* m_vkComputeRenderTarget;

	// Storage buffers for spheres and planes
	storageBuffer m_vkStorageBuffers;
	// Uniform buffer
	VKBuffer m_vkUniformBuffer;			
	// Queue for compute commands
	VkQueue m_vkQueue;
	// Seperate Command pool for compute
	VkCommandPool m_vkCommandPool;
	// Command buffer used for storing dispatch commands
	VkCommandBuffer m_vkCommandBuffer;
	// Sync fence to avoid rewriting compute if its still being used.
	VkFence m_vkFence;
	// Compute shader binding layout
	VkDescriptorSetLayout m_vkDescriptorSetLayout;
	// Compute shader bindings
	VkDescriptorSet m_vkDescriptorSet;
	// Layout of compute pipeline
	VkPipelineLayout m_vkPipelineLayout;
	// Compute pipeline
	VkPipeline m_vkPipeline;
	// Compute shader uniform block object
	struct UBOCompute 
	{							
		glm::vec3 lightPos;
		float aspectRatio;						// Aspect ratio of the viewport
		glm::vec4 fogColor = glm::vec4(0.0f);
		struct 
		{
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 4.0f);
			glm::vec3 lookat = glm::vec3(0.0f, 0.5f, 0.0f);
			float fov = 10.0f;
		} camera;
	} ubo;


	void PrepareStorageBuffers();
	void PrepareUniformBuffers();
	void PrepareTextureTarget(VKTexture *tex, uint32_t width, uint32_t height, VkFormat format);
	void SetupDescriptorSetLayout();
	void PreparePipelines();
	void SetupDescriptorPool();
	void SetupDescriptorSet();
	void PrepareCompute();
	void BuildCommandBuffers();

	// Helpers
	void UpdateUniformBuffers();


	// Descriptor Layout
	VkDescriptorSetLayout* m_vkDescriptorSetLayout;
	// Descriptor Pool
	VkDescriptorPool* m_vkDescriptorPool;
	// Descriptor sets
	std::vector<VkDescriptorSet> m_vkDescriptorSets;
	// Pipeline Layout
	VkPipelineLayout* m_vkPipelineLayout;
	// Compute Pipeline Layout
	VkPipelineLayout* m_vkComputePipelineLayout;
	// Graphics Pipeline
	VkPipeline* m_vkPipeline;
	// Texture Sampler
	VkSampler* m_vkTextureSampler;

	//// Creates a shader module
	//VkShaderModule CreateShaderModule(VkDevice _vkDevice,
	//	const std::vector<char>& _code);
	//// Create Pipeline
	//void CreatePipelineLayout(VkExtent2D _vkSwapChainExtent,
	//	const char* _vertexFilePath,
	//	const char * _fragmentFilePath,
	//	const char* _computeFilePath);
	//// Create compute module
	//void CreateComputeModule(std::vector<char> _computeCode);
	//// Create the Render Pass
	//void CreateRenderPass(VkFormat _vkSwapChainImageFormat);
	//// Create Descriptor Sets.
	//void CreateDescriptorSetLayout();
	//// Create Descriptor Pools.
	//void CreateDescriptorPools();
	//// Create Descriptor Sets.
	//void CreateDescriptorSets(VKObject * _vkObject);
	//// Creates texture sampler.
	//void CreateTextureSampler();
public:
	VKPipeline(VKEngine* _vkEngine,
		VkExtent2D _vkSwapChainExtent,
		VkFormat _vkSwapChainImageFormat,
		const char* _vertexFilePath,
		const char* _fragmentFilePath, 
		const char* _computeFilePath);
	//VkRenderPass vkGetRenderPass() { return m_vkRenderPass; }
	VkPipeline vkGetPipeline() { return m_vkPipeline; }
	VkPipelineLayout vkGetPipelineLayout() { return m_vkPipelineLayout; }
	std::vector<VkDescriptorSet> vkGetDescriptorSets() { return m_vkDescriptorSets; }
	~VKPipeline();
};

#endif