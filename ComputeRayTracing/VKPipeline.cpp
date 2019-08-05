#if VK
#include "VKPipeline.h"
#include "Vertex.h"
#include "Window.h"

// Init Vulkan Shader Component
VKPipeline::VKPipeline(VkDevice* _vkDevice,
	VkExtent2D _vkSwapChainExtent,
	VkFormat _vkSwapChainImageFormat,
	const char* _vertexFilePath, 
	const char * _fragmentFilePath)
{
	// Pointer to device to destroy later
	m_vkDevice = _vkDevice;
	// Create Render Pass
	CreateRenderPass(_vkSwapChainImageFormat);
	// Create descriptor set
	CreateDescriptorSets();
	// Create Pipeline
	CreatePipelineLayout(_vkSwapChainExtent, _vertexFilePath, _fragmentFilePath);
}

VKPipeline::~VKPipeline()
{
	// Destroy pipeline
	vkDestroyPipelineLayout(*m_vkDevice, *m_vkPipelineLayout, nullptr);
	// Destroy render pass
	vkDestroyRenderPass(*m_vkDevice, *m_vkRenderPass, nullptr);
	// Destroy descriptor sets
	vkDestroyDescriptorSetLayout(*m_vkDevice, *m_vkDescriptorSetLayout, nullptr);
	// Destroy Pipeline
	vkDestroyPipeline(*m_vkDevice, *m_vkPipeline, nullptr);
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
	VkShaderModule fragShaderModule = CreateShaderModule(*m_vkDevice, fragShaderCode);
	VkShaderModule vertShaderModule = CreateShaderModule(*m_vkDevice, vertShaderCode);

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
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	// Disable Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

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

	if (vkCreatePipelineLayout(*m_vkDevice, &pipelineLayoutInfo, nullptr, m_vkPipelineLayout)
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
	if (vkCreateGraphicsPipelines(*m_vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, m_vkPipeline)
		!= VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline!");
	}
	
	// Destroy shader modules at the end
	vkDestroyShaderModule(*m_vkDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(*m_vkDevice, vertShaderModule, nullptr);
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

	// Create color attachment reference
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Subpass description
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	// Specify color attachment
	// => layout(location = 0) out vec4 outColor in the shader
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// Render Pass Create Info
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	// Create the render pass
	m_vkRenderPass = new VkRenderPass();
	if (vkCreateRenderPass(*m_vkDevice, &renderPassInfo, nullptr, m_vkRenderPass) 
		!= VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create render pass!");
	}


}

void VKPipeline::CreateDescriptorSets()
{
	// Descriptor set binding info
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	// Describe shader stage the uniform object is for
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
	// Layout info
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;
	// Create the desacriptor ste layout
	m_vkDescriptorSetLayout = new VkDescriptorSetLayout();
	if (vkCreateDescriptorSetLayout(*m_vkDevice, &layoutInfo, nullptr, m_vkDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}


#endif