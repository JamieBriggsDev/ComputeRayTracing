#if VK
#include "VKPipeline.h"
#include "VKObject.h"
#include "VKTexture.h"
#include "Vertex.h"
#include "Window.h"

// Init Vulkan Shader Component
VKPipeline::VKPipeline(VKEngine* _vkEngine,
	VkExtent2D _vkSwapChainExtent,
	VkFormat _vkSwapChainImageFormat,
	const char* _vertexFilePath,
	const char* _fragmentFilePath, 
	VKObject* _object)
{
	// Pointer to device to destroy later
	m_vkEngineRef = _vkEngine;
	// Create Render Pass
	CreateRenderPass(_vkSwapChainImageFormat);
	// Create texture samplers
	CreateTextureSampler();
	// Create descriptor set layout
	CreateDescriptorSetLayout();
	// Create Pipeline
	CreatePipelineLayout(_vkSwapChainExtent, 
		_vertexFilePath, 
		_fragmentFilePath);
	// Create descriptor pools
	CreateDescriptorPools();
	// Create descriptor set
	CreateDescriptorSets(_object);
}

VKPipeline::~VKPipeline()
{
	// Destroy pipeline
	vkDestroyPipelineLayout(*m_vkEngineRef->vkGetDevice(), *m_vkPipelineLayout, nullptr);
	// Destroy render pass
	vkDestroyRenderPass(*m_vkEngineRef->vkGetDevice(), *m_vkRenderPass, nullptr);
	// Destroy descriptor sets
	vkDestroyDescriptorSetLayout(*m_vkEngineRef->vkGetDevice(), *m_vkDescriptorSetLayout, nullptr);
	// Destroy descriptor pool
	vkDestroyDescriptorPool(*m_vkEngineRef->vkGetDevice(), *m_vkDescriptorPool, nullptr);
	// Destroy Pipeline
	vkDestroyPipeline(*m_vkEngineRef->vkGetDevice(), *m_vkPipeline, nullptr);
	// Destroy samplers
	vkDestroySampler(*m_vkEngineRef->vkGetDevice(), *m_vkTextureSampler, nullptr);
}

VkShaderModule VKPipeline::CreateShaderModule(VkDevice _vkDevice, const std::vector<char>& _code)
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

void VKPipeline::CreatePipelineLayout(VkExtent2D _vkSwapChainExtent,
	const char* _vertexFilePath,
	const char * _fragmentFilePath)
{
	// Get the shader code.
	std::vector<char> fragShaderCode = ReadFile(_fragmentFilePath);
	std::vector<char> vertShaderCode = ReadFile(_vertexFilePath);

	// Create the shader modules
	VkShaderModule fragShaderModule = CreateShaderModule(*m_vkEngineRef->vkGetDevice(), fragShaderCode);
	VkShaderModule vertShaderModule = CreateShaderModule(*m_vkEngineRef->vkGetDevice(), vertShaderCode);

	// Vertex shader stage create info
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	// Frag shader stage create info
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	// Shader stages
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	// Vertex input state create info
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	// Input assemby state create info
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Setup Viewport
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)_vkSwapChainExtent.width;
	viewport.height = (float)_vkSwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	// Setup Scissor
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = _vkSwapChainExtent;
	// Create pipeline viewport state
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// Setup Rasterizer
	//VkPipelineRasterizationStateCreateInfo rasterizer = {};
	//rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	//rasterizer.depthClampEnable = VK_FALSE;
	//rasterizer.rasterizerDiscardEnable = VK_FALSE;
	//rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	//rasterizer.lineWidth = 1.0f;
	//// Choose backwards culling
	//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	//rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	//// Not doing shadow mapping so turning depth bias off
	//rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	// Disable Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Setup depth stencil
	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	// Setup Color Blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	// Setup dynamic state
	VkDynamicState dynamicStates[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	// Finally create the pipeline
	m_vkPipelineLayout = new VkPipelineLayout();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = m_vkDescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(*m_vkEngineRef->vkGetDevice(), &pipelineLayoutInfo, nullptr, m_vkPipelineLayout)
		!= VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	// Create Pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	// Reference all the structs created beforehand.
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	// Reference pipeline layout
	pipelineInfo.layout = *m_vkPipelineLayout;
	// Reference render pass
	pipelineInfo.renderPass = *m_vkRenderPass;
	pipelineInfo.subpass = 0;
	// Dont need pipelineHandle or index
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	// Create graphics pipline
	m_vkPipeline = new VkPipeline();
	if (vkCreateGraphicsPipelines(*m_vkEngineRef->vkGetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, m_vkPipeline)
		!= VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline!");
	}
	
	// Destroy shader modules at the end
	vkDestroyShaderModule(*m_vkEngineRef->vkGetDevice(), fragShaderModule, nullptr);
	vkDestroyShaderModule(*m_vkEngineRef->vkGetDevice(), vertShaderModule, nullptr);
}

void VKPipeline::CreateRenderPass(VkFormat _vkSwapChainImageFormat)
{
	// Create Color Attachment
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = _vkSwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	// Decide what to do with data before and after rendering:
	// - Preserve the existing contents of the attachment
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// - Rendered contents will be stored in memory and can be read later.
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Not using scencil data
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Initial layout isn't defined
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Create depth Attachment
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = m_vkEngineRef->vkFindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Create color attachment reference
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Create depth attachment reference
	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Subpass description
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	// Specify color attachment
	// => layout(location = 0) out vec4 outColor in the shader
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// Render Pass Create Info
	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	// Create the render pass
	m_vkRenderPass = new VkRenderPass();
	if (vkCreateRenderPass(*m_vkEngineRef->vkGetDevice(), &renderPassInfo, nullptr, m_vkRenderPass) 
		!= VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create render pass!");
	}


}

void VKPipeline::CreateDescriptorSetLayout()
{
	// Descriptor set binding info
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	// Describe shader stage the uniform object is for
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	// Sampler layout binding info
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	// Layout info
	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
	layoutInfo.pBindings = bindings.data();
	// Create the desacriptor ste layout
	m_vkDescriptorSetLayout = new VkDescriptorSetLayout();
	if (vkCreateDescriptorSetLayout(*m_vkEngineRef->vkGetDevice(), &layoutInfo, nullptr, m_vkDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void VKPipeline::CreateDescriptorPools()
{
	// Descriptor pool size info
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(m_vkEngineRef->vkGetSwapChainImages().size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(m_vkEngineRef->vkGetSwapChainImages().size());
	// Descriptor pool create info
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	// Define max sets.
	poolInfo.maxSets = static_cast<uint32_t>(m_vkEngineRef->vkGetSwapChainImages().size());

	// Create the descriptor pool.
	m_vkDescriptorPool = new VkDescriptorPool();
	if (vkCreateDescriptorPool(*m_vkEngineRef->vkGetDevice(), 
		&poolInfo, 
		nullptr, 
		m_vkDescriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void VKPipeline::CreateDescriptorSets(VKObject *_vkObject)
{
	size_t SwapChainImageSize = m_vkEngineRef->vkGetSwapChainImages().size();
	// Descriptor sets layout allocate info
	std::vector<VkDescriptorSetLayout> layouts(SwapChainImageSize, *m_vkDescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = *m_vkDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapChainImageSize);
	allocInfo.pSetLayouts = layouts.data();
	// Allocate Descirptor Sets.
	m_vkDescriptorSets.resize(SwapChainImageSize);
	if (vkAllocateDescriptorSets(*m_vkEngineRef->vkGetDevice(), &allocInfo, m_vkDescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	// Insert data into descriptor sets.
	for (size_t i = 0; i < SwapChainImageSize; i++)
	{
		// Descriptor buffer info.
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = static_cast<VKModel*>(_vkObject->GetModel())->vkGetUniformBuffers()[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);
		// Descriptor image info
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = *static_cast<VKTexture*>(_vkObject->GetTexture())->GetTextureImageView();
		imageInfo.sampler = *m_vkTextureSampler;

		// Write descriptor sets.
		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
		// uniform buffer descriptor sets
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_vkDescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		// texture sampler descriptor sets
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_vkDescriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;
		// Update descriptor sets.
		vkUpdateDescriptorSets(*m_vkEngineRef->vkGetDevice(),
			static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 
			0, nullptr);
	}
}

void VKPipeline::CreateTextureSampler()
{
	// Texture sampler create info.
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	// Sampler address mode.
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// Anisotropy settings.
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 1;
	// Border color
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	// Comparison functions disabled
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	// Mipmap mode
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	// Create sampler
	m_vkTextureSampler = new VkSampler();
	if (vkCreateSampler(*m_vkEngineRef->vkGetDevice(), &samplerInfo,
		nullptr, m_vkTextureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture sampler!");
	}
}

#endif