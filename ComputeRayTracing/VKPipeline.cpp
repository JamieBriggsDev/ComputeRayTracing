#if VK
#include "VKPipeline.h"
#include "VKObject.h"
#include "VKTexture.h"
#include "Vertex.h"
#include "Window.h"
#include "Shapes.h"

// Init Vulkan Shader Component
VKPipeline::VKPipeline(VKEngine* _vkEngine,
	VkExtent2D _vkSwapChainExtent,
	VkFormat _vkSwapChainImageFormat,
	const char* _vertexFilePath,
	const char* _fragmentFilePath,
	const char* _computeFilePath)
{
	// Pointer to device to destroy later
	m_vkEngineRef = _vkEngine;
	// Compute render target
	//m_vkComputeRenderTarget = new VKTexture();

	// Done
	PrepareStorageBuffers();
	PrepareUniformBuffers();
	PrepareTextureTarget(m_vkComputeRenderTarget, 
		WINDOW_WIDTH, WINDOW_HEIGHT, 
		VK_FORMAT_R8G8B8A8_UNORM);

	// Preparing
	SetupDescriptorSetLayout();

	// Not implemented
	PreparePipelines();
	SetupDescriptorPool();
	SetupDescriptorSet();
	PrepareCompute();
	BuildCommandBuffers();

	//// Create Render Pass
	//CreateRenderPass(_vkSwapChainImageFormat);
	//// Create texture samplers
	//CreateTextureSampler();
	//// Create descriptor set layout
	//CreateDescriptorSetLayout();
	//// Create Pipeline
	//CreatePipelineLayout(_vkSwapChainExtent,
	//	_vertexFilePath,
	//	_fragmentFilePath,
	//	_computeFilePath);
	//// Create descriptor pools
	//CreateDescriptorPools();
	//// Create descriptor set
	//CreateDescriptorSets(_object);
}

VKPipeline::~VKPipeline()
{
	// Destroy pipeline
	vkDestroyPipelineLayout(*m_vkEngineRef->vkGetDevice(), *m_vkPipelineLayout, nullptr);
	// Destroy descriptor sets
	vkDestroyDescriptorSetLayout(*m_vkEngineRef->vkGetDevice(), *m_vkDescriptorSetLayout, nullptr);
	// Destroy descriptor pool
	vkDestroyDescriptorPool(*m_vkEngineRef->vkGetDevice(), *m_vkDescriptorPool, nullptr);
	// Destroy Pipeline
	vkDestroyPipeline(*m_vkEngineRef->vkGetDevice(), *m_vkPipeline, nullptr);
	// Destroy samplers
	vkDestroySampler(*m_vkEngineRef->vkGetDevice(), *m_vkTextureSampler, nullptr);
}

void VKPipeline::PrepareStorageBuffers()
{
	// Spheres
	std::vector<Sphere> spheres;
	spheres.push_back(newSphere(glm::vec3(1.75f, -0.5f, 0.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f), 32.0f, m_totalObjects));
	spheres.push_back(newSphere(glm::vec3(0.0f, 1.0f, -0.5f), 1.0f, glm::vec3(0.65f, 0.77f, 0.97f), 32.0f, m_totalObjects));
	spheres.push_back(newSphere(glm::vec3(-1.75f, -0.75f, -0.5f), 1.25f, glm::vec3(0.9f, 0.76f, 0.46f), 32.0f, m_totalObjects));
	VkDeviceSize storageBufferSize = spheres.size() * sizeof(Sphere);

	// Stage
	VKBuffer stagingBuffer;

	m_vkEngineRef->vkSetupBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer,
		storageBufferSize,
		spheres.data());

	m_vkEngineRef->vkSetupBuffer(
		// The SSBO will be used as a storage buffer for the compute pipeline and as a vertex buffer in the graphics pipeline
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&m_vkStorageBuffers.spheres,
		storageBufferSize);

	// Copy to staging buffer
	VkCommandBuffer copyCmd = m_vkEngineRef->vkBeginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
	VkBufferCopy copyRegion = {};
	copyRegion.size = storageBufferSize;
	vkCmdCopyBuffer(copyCmd, stagingBuffer.buffer, m_vkStorageBuffers.spheres.buffer, 1, &copyRegion);
	m_vkEngineRef->vkEndSingleTimeCommands(copyCmd, m_vkQueue, true);

	stagingBuffer.destroy();

	// Planes
	std::vector<Plane> planes;
	const float roomDim = 4.0f;
	planes.push_back(newPlane(glm::vec3(0.0f, 1.0f, 0.0f), roomDim, glm::vec3(1.0f), 32.0f, m_totalObjects));
	planes.push_back(newPlane(glm::vec3(0.0f, -1.0f, 0.0f), roomDim, glm::vec3(1.0f), 32.0f, m_totalObjects));
	planes.push_back(newPlane(glm::vec3(0.0f, 0.0f, 1.0f), roomDim, glm::vec3(1.0f), 32.0f, m_totalObjects));
	planes.push_back(newPlane(glm::vec3(0.0f, 0.0f, -1.0f), roomDim, glm::vec3(0.0f), 32.0f, m_totalObjects));
	planes.push_back(newPlane(glm::vec3(-1.0f, 0.0f, 0.0f), roomDim, glm::vec3(1.0f, 0.0f, 0.0f), 32.0f, m_totalObjects));
	planes.push_back(newPlane(glm::vec3(1.0f, 0.0f, 0.0f), roomDim, glm::vec3(0.0f, 1.0f, 0.0f), 32.0f, m_totalObjects));
	storageBufferSize = planes.size() * sizeof(Plane);

	// Stage
	m_vkEngineRef->vkSetupBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer,
		storageBufferSize,
		planes.data());

	m_vkEngineRef->vkSetupBuffer(
		// The SSBO will be used as a storage buffer for the compute pipeline and as a vertex buffer in the graphics pipeline
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&m_vkStorageBuffers.planes,
		storageBufferSize);

	// Copy to staging buffer
	copyCmd = m_vkEngineRef->vkBeginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
	copyRegion.size = storageBufferSize;
	vkCmdCopyBuffer(copyCmd, stagingBuffer.buffer, m_vkStorageBuffers.planes.buffer, 1, &copyRegion);
	m_vkEngineRef->vkEndSingleTimeCommands(copyCmd, m_vkQueue, true);

	stagingBuffer.destroy();
}

void VKPipeline::PrepareUniformBuffers()
{
	// Creates uniform buffer
	m_vkEngineRef->vkSetupBuffer(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&m_vkUniformBuffer,
		sizeof(ubo));

	UpdateUniformBuffers();
}

void VKPipeline::PrepareTextureTarget(VKTexture* tex, uint32_t width, uint32_t height, VkFormat format)
{
	// Get device properties for the requested texture format
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(*m_vkEngineRef->vkGetPhysicalDevice(), format, &formatProperties);
	// Check if requested image format supports image storage operations
	assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);

	// Prepare blit target texture
	tex->width = width;
	tex->height = height;

	VkImageCreateInfo imageCreateInfo {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = format;
	imageCreateInfo.extent = { width, height, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Image will be sampled in the fragment shader and used as storage target in the compute shader
	imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	imageCreateInfo.flags = 0;

	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	VkMemoryRequirements memReqs;

	if (vkCreateImage(*m_vkEngineRef->vkGetDevice(), &imageCreateInfo, nullptr, &tex->image) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create image!");
	}

	vkGetImageMemoryRequirements(*m_vkEngineRef->vkGetDevice(), tex->image, &memReqs);
	memAllocInfo.allocationSize = memReqs.size;
	memAllocInfo.memoryTypeIndex = m_vkEngineRef->vkFindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	// Allocate memory
	if (vkAllocateMemory(*m_vkEngineRef->vkGetDevice(), &memAllocInfo, nullptr, &tex->deviceMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not allocate memory!");
	}
	// Bind image memory
	if (vkBindImageMemory(*m_vkEngineRef->vkGetDevice(), tex->image, tex->deviceMemory, 0) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not bind image memory!");
	}

	VkCommandBuffer layoutCmd = m_vkEngineRef->vkBeginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

	tex->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	VKTexture::SetImageLayout(
		layoutCmd,
		tex->image,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		tex->imageLayout);

	m_vkEngineRef->vkEndSingleTimeCommands(layoutCmd, m_vkQueue, true);

	// Create sampler
	tex->CreateSampler(*m_vkEngineRef->vkGetDevice());

	// Create image view
	tex->CreateImageView(*m_vkEngineRef->vkGetDevice(), format);

	// Initialize a descriptor for later use
	tex->descriptor.imageLayout = tex->imageLayout;
	tex->descriptor.imageView = tex->view;
	tex->descriptor.sampler = tex->sampler;
	tex->device = m_vkEngineRef->vkGetDevice();
}

void VKPipeline::SetupDescriptorSetLayout()
{
	// Descriptor set layout
	VkDescriptorSetLayoutBinding setLayoutBinding{};
	setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	setLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	setLayoutBinding.binding = 0;
	setLayoutBinding.descriptorCount = 1;
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
	{
		// Binding 0 : Fragment shader image sampler
		setLayoutBinding
	};

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();
	descriptorSetLayoutCreateInfo.bindingCount = setLayoutBindings.size();


	if (vkCreateDescriptorSetLayout(*m_vkEngineRef->vkGetDevice(),
		&descriptorSetLayoutCreateInfo, nullptr,
		m_vkDescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor sets.");
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = m_vkDescriptorSetLayout;

	if (vkCreatePipelineLayout(*m_vkEngineRef->vkGetDevice(), &pipelineLayoutCreateInfo, nullptr, m_vkPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout.");
	}
}

void VKPipeline::PreparePipelines()
{
}

void VKPipeline::SetupDescriptorPool()
{
}

void VKPipeline::SetupDescriptorSet()
{
}

void VKPipeline::PrepareCompute()
{
}

void VKPipeline::BuildCommandBuffers()
{
}

void VKPipeline::UpdateUniformBuffers()
{
	float timer = m_vkEngineRef->GetTimer();
	ubo.lightPos.x = 0.0f + sin(glm::radians(timer * 360.0f)) * cos(glm::radians(timer * 360.0f)) * 2.0f;
	ubo.lightPos.y = 0.0f + sin(glm::radians(timer * 360.0f)) * 2.0f;
	ubo.lightPos.z = 0.0f + cos(glm::radians(timer * 360.0f)) * 2.0f;
	ubo.camera.pos = glm::vec3(0.0f, 0.0f, -5.0f);// *-1.0f; TODO
	m_vkUniformBuffer.map();
	memcpy(m_vkUniformBuffer.mapped, &ubo, sizeof(ubo));
	m_vkUniformBuffer.unmap();
}




#pragma region old

//
//VkShaderModule VKPipeline::CreateShaderModule(VkDevice _vkDevice, const std::vector<char>& _code)
//{
//	// Shader module create info.
//	VkShaderModuleCreateInfo createInfo = {};
//	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//	createInfo.codeSize = _code.size();
//	createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());
//	// Create shader module.
//	VkShaderModule shaderModule;
//	if (vkCreateShaderModule(_vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
//	{
//		throw std::runtime_error("Failed to create shader module!");
//	}
//	// Return the shader module.
//	return shaderModule;
//}
//
//void VKPipeline::CreatePipelineLayout(VkExtent2D _vkSwapChainExtent,
//	const char* _vertexFilePath,
//	const char * _fragmentFilePath,
//	const char* _computeFilePath)
//{
//	// Get the shader code.
//	std::vector<char> fragShaderCode = ReadFile(_fragmentFilePath);
//	std::vector<char> vertShaderCode = ReadFile(_vertexFilePath);
//	std::vector<char> compShaderCode = ReadFile(_computeFilePath);
//
//	// Create the shader modules
//	VkShaderModule fragShaderModule = CreateShaderModule(*m_vkEngineRef->vkGetDevice(), fragShaderCode);
//	VkShaderModule vertShaderModule = CreateShaderModule(*m_vkEngineRef->vkGetDevice(), vertShaderCode);
//
//	// Setup compute shader module
//	CreateComputeModule(compShaderCode);
//
//	// Vertex shader stage create info
//	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
//	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//	vertShaderStageInfo.module = vertShaderModule;
//	vertShaderStageInfo.pName = "main";
//	// Frag shader stage create info
//	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
//	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//	fragShaderStageInfo.module = fragShaderModule;
//	fragShaderStageInfo.pName = "main";
//
//	// Shader stages
//	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
//
//	// Vertex input state create info
//	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
//	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//
//	auto bindingDescription = Vertex::getBindingDescription();
//	auto attributeDescriptions = Vertex::getAttributeDescriptions();
//	vertexInputInfo.vertexBindingDescriptionCount = 1;
//	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
//	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
//	// Input assemby state create info
//	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
//	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//	inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//	// Setup Viewport
//	VkViewport viewport = {};
//	viewport.x = 0.0f;
//	viewport.y = 0.0f;
//	viewport.width = (float)_vkSwapChainExtent.width;
//	viewport.height = (float)_vkSwapChainExtent.height;
//	viewport.minDepth = 0.0f;
//	viewport.maxDepth = 1.0f;
//	// Setup Scissor
//	VkRect2D scissor = {};
//	scissor.offset = { 0, 0 };
//	scissor.extent = _vkSwapChainExtent;
//	// Create pipeline viewport state
//	VkPipelineViewportStateCreateInfo viewportState = {};
//	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//	viewportState.viewportCount = 1;
//	viewportState.pViewports = &viewport;
//	viewportState.scissorCount = 1;
//	viewportState.pScissors = &scissor;
//
//	VkPipelineRasterizationStateCreateInfo rasterizer = {};
//	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//	rasterizer.depthClampEnable = VK_FALSE;
//	rasterizer.rasterizerDiscardEnable = VK_FALSE;
//	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//	rasterizer.lineWidth = 1.0f;
//	rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
//	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//	rasterizer.depthBiasEnable = VK_FALSE;
//
//	// Disable Multisampling
//	VkPipelineMultisampleStateCreateInfo multisampling = {};
//	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//	multisampling.sampleShadingEnable = VK_FALSE;
//	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//
//	// Setup Color Blending
//	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
//	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
//		VK_COLOR_COMPONENT_G_BIT |
//		VK_COLOR_COMPONENT_B_BIT |
//		VK_COLOR_COMPONENT_A_BIT;
//	colorBlendAttachment.blendEnable = VK_TRUE;
//	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
//	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
//	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
//	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
//
//	VkPipelineColorBlendStateCreateInfo colorBlending = {};
//	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//	colorBlending.logicOpEnable = VK_FALSE;
//	colorBlending.logicOp = VK_LOGIC_OP_COPY;
//	colorBlending.attachmentCount = 1;
//	colorBlending.pAttachments = &colorBlendAttachment;
//	colorBlending.blendConstants[0] = 0.0f;
//	colorBlending.blendConstants[1] = 0.0f;
//	colorBlending.blendConstants[2] = 0.0f;
//	colorBlending.blendConstants[3] = 0.0f;
//
//	// Setup dynamic state
//	VkDynamicState dynamicStates[] = {
//	VK_DYNAMIC_STATE_VIEWPORT,
//	VK_DYNAMIC_STATE_LINE_WIDTH
//	};
//
//	VkPipelineDynamicStateCreateInfo dynamicState = {};
//	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//	dynamicState.dynamicStateCount = 2;
//	dynamicState.pDynamicStates = dynamicStates;
//
//	// Finally create the pipeline
//	m_vkPipelineLayout = new VkPipelineLayout();
//
//	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
//	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//	pipelineLayoutInfo.setLayoutCount = 1;
//	pipelineLayoutInfo.pSetLayouts = m_vkDescriptorSetLayout;
//	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
//	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
//
//	if (vkCreatePipelineLayout(*m_vkEngineRef->vkGetDevice(), &pipelineLayoutInfo, nullptr, m_vkPipelineLayout)
//		!= VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create pipeline layout!");
//	}
//
//	// Create Pipeline
//	VkGraphicsPipelineCreateInfo pipelineInfo = {};
//	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//	pipelineInfo.stageCount = 2;
//	pipelineInfo.pStages = shaderStages;
//	// Reference all the structs created beforehand.
//	pipelineInfo.pVertexInputState = &vertexInputInfo;
//	pipelineInfo.pInputAssemblyState = &inputAssembly;
//	pipelineInfo.pViewportState = &viewportState;
//	pipelineInfo.pRasterizationState = &rasterizer;
//	pipelineInfo.pMultisampleState = &multisampling;
//	pipelineInfo.pColorBlendState = &colorBlending;
//	// Reference pipeline layout
//	pipelineInfo.layout = *m_vkPipelineLayout;
//	// Reference render pass
//	pipelineInfo.renderPass = *m_vkRenderPass;
//	pipelineInfo.subpass = 0;
//	// Dont need pipelineHandle or index
//	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
//	pipelineInfo.basePipelineIndex = -1; // Optional
//
//	// Create graphics pipline
//	m_vkPipeline = new VkPipeline();
//	if (vkCreateGraphicsPipelines(*m_vkEngineRef->vkGetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, m_vkPipeline)
//		!= VK_SUCCESS)
//	{
//		throw std::runtime_error("Failed to create graphics pipeline!");
//	}
//
//	// Destroy shader modules at the end
//	vkDestroyShaderModule(*m_vkEngineRef->vkGetDevice(), fragShaderModule, nullptr);
//	vkDestroyShaderModule(*m_vkEngineRef->vkGetDevice(), vertShaderModule, nullptr);
//}
//
//void VKPipeline::CreateComputeModule(std::vector<char> _computeCode)
//{
//	// Create shader module
//	VkShaderModule compShaderModule = CreateShaderModule(*m_vkEngineRef->vkGetDevice(), _computeCode);
//	// Shader stage create info
//	VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
//	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//	shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
//	shaderStageCreateInfo.module = compShaderModule;
//	shaderStageCreateInfo.pName = "main";
//	// Pipeline layout module
//	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
//	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//	pipelineLayoutCreateInfo.setLayoutCount = 1;
//	pipelineLayoutCreateInfo.pSetLayouts = m_vkDescriptorSetLayout;
//	m_vkComputePipelineLayout = new VkPipelineLayout();
//
//	if (vkCreatePipelineLayout(*m_vkEngineRef->vkGetDevice(), &pipelineLayoutCreateInfo, nullptr, m_vkComputePipelineLayout) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create compute pipeline layout!");
//	}
//}
//
//void VKPipeline::CreateRenderPass(VkFormat _vkSwapChainImageFormat)
//{
//	// Create Color Attachment
//	VkAttachmentDescription colorAttachment = {};
//	colorAttachment.format = _vkSwapChainImageFormat;
//	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//	// Decide what to do with data before and after rendering:
//	// - Preserve the existing contents of the attachment
//	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//	// - Rendered contents will be stored in memory and can be read later.
//	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//	// Not using scencil data
//	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//	// Initial layout isn't defined
//	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//
//	// Create color attachment reference
//	VkAttachmentReference colorAttachmentRef = {};
//	colorAttachmentRef.attachment = 0;
//	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//	// Subpass description
//	VkSubpassDescription subpass = {};
//	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//	// Specify color attachment
//	// => layout(location = 0) out vec4 outColor in the shader
//	subpass.colorAttachmentCount = 1;
//	subpass.pColorAttachments = &colorAttachmentRef;
//
//	VkSubpassDependency dependency = {};
//	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//	dependency.dstSubpass = 0;
//	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//	dependency.srcAccessMask = 0;
//	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//
//	// Render Pass Create Info
//	VkRenderPassCreateInfo renderPassInfo = {};
//	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//	renderPassInfo.attachmentCount = 1;
//	renderPassInfo.pAttachments = &colorAttachment;
//	renderPassInfo.subpassCount = 1;
//	renderPassInfo.pSubpasses = &subpass;
//	renderPassInfo.dependencyCount = 1;
//	renderPassInfo.pDependencies = &dependency;
//
//	// Create the render pass
//	m_vkRenderPass = new VkRenderPass();
//	if (vkCreateRenderPass(*m_vkEngineRef->vkGetDevice(), &renderPassInfo, nullptr, m_vkRenderPass)
//		!= VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create render pass!");
//	}
//
//
//}
//
//void VKPipeline::CreateDescriptorSetLayout()
//{
//	// Descriptor set binding info
//	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
//	uboLayoutBinding.binding = 0;
//	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	uboLayoutBinding.descriptorCount = 1;
//	// Describe shader stage the uniform object is for
//	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
//
//	// Sampler layout binding info
//	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
//	samplerLayoutBinding.binding = 1;
//	samplerLayoutBinding.descriptorCount = 1;
//	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	samplerLayoutBinding.pImmutableSamplers = nullptr;
//	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//
//	// Layout info
//	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
//	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
//	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
//	layoutInfo.pBindings = bindings.data();
//	// Create the desacriptor ste layout
//	m_vkDescriptorSetLayout = new VkDescriptorSetLayout();
//	if (vkCreateDescriptorSetLayout(*m_vkEngineRef->vkGetDevice(), &layoutInfo, nullptr, m_vkDescriptorSetLayout) != VK_SUCCESS) {
//		throw std::runtime_error("failed to create descriptor set layout!");
//	}
//}
//
//void VKPipeline::CreateDescriptorPools()
//{
//	// Descriptor pool size info
//	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
//	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	poolSizes[0].descriptorCount = static_cast<uint32_t>(m_vkEngineRef->vkGetSwapChainImages().size());
//	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	poolSizes[1].descriptorCount = static_cast<uint32_t>(m_vkEngineRef->vkGetSwapChainImages().size());
//	// Descriptor pool create info
//	VkDescriptorPoolCreateInfo poolInfo = {};
//	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//	poolInfo.pPoolSizes = poolSizes.data();
//	// Define max sets.
//	poolInfo.maxSets = static_cast<uint32_t>(m_vkEngineRef->vkGetSwapChainImages().size());
//
//	// Create the descriptor pool.
//	m_vkDescriptorPool = new VkDescriptorPool();
//	if (vkCreateDescriptorPool(*m_vkEngineRef->vkGetDevice(),
//		&poolInfo,
//		nullptr,
//		m_vkDescriptorPool) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create descriptor pool!");
//	}
//}
//
//void VKPipeline::CreateDescriptorSets(VKObject *_vkObject)
//{
//	size_t SwapChainImageSize = m_vkEngineRef->vkGetSwapChainImages().size();
//	// Descriptor sets layout allocate info
//	std::vector<VkDescriptorSetLayout> layouts(SwapChainImageSize, *m_vkDescriptorSetLayout);
//	VkDescriptorSetAllocateInfo allocInfo = {};
//	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//	allocInfo.descriptorPool = *m_vkDescriptorPool;
//	allocInfo.descriptorSetCount = static_cast<uint32_t>(SwapChainImageSize);
//	allocInfo.pSetLayouts = layouts.data();
//	// Allocate Descirptor Sets.
//	m_vkDescriptorSets.resize(SwapChainImageSize);
//	if (vkAllocateDescriptorSets(*m_vkEngineRef->vkGetDevice(), &allocInfo, m_vkDescriptorSets.data()) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to allocate descriptor sets!");
//	}
//	// Insert data into descriptor sets.
//	for (size_t i = 0; i < SwapChainImageSize; i++)
//	{
//		// Descriptor buffer info.
//		VkDescriptorBufferInfo bufferInfo = {};
//		bufferInfo.buffer = static_cast<VKModel*>(_vkObject->GetModel())->vkGetUniformBuffers()[i];
//		bufferInfo.offset = 0;
//		bufferInfo.range = sizeof(UniformBufferObject);
//		// Descriptor image info
//		VkDescriptorImageInfo imageInfo = {};
//		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//		imageInfo.imageView = *static_cast<VKTexture*>(_vkObject->GetTexture())->GetTextureImageView();
//		imageInfo.sampler = *m_vkTextureSampler;
//
//		// Write descriptor sets.
//		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
//		// uniform buffer descriptor sets
//		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//		descriptorWrites[0].dstSet = m_vkDescriptorSets[i];
//		descriptorWrites[0].dstBinding = 0;
//		descriptorWrites[0].dstArrayElement = 0;
//		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		descriptorWrites[0].descriptorCount = 1;
//		descriptorWrites[0].pBufferInfo = &bufferInfo;
//		// texture sampler descriptor sets
//		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//		descriptorWrites[1].dstSet = m_vkDescriptorSets[i];
//		descriptorWrites[1].dstBinding = 1;
//		descriptorWrites[1].dstArrayElement = 0;
//		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//		descriptorWrites[1].descriptorCount = 1;
//		descriptorWrites[1].pImageInfo = &imageInfo;
//		// Update descriptor sets.
//		vkUpdateDescriptorSets(*m_vkEngineRef->vkGetDevice(),
//			static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
//			0, nullptr);
//	}
//}
//
//void VKPipeline::CreateTextureSampler()
//{
//	// Texture sampler create info.
//	VkSamplerCreateInfo samplerInfo = {};
//	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//	samplerInfo.magFilter = VK_FILTER_LINEAR;
//	samplerInfo.minFilter = VK_FILTER_LINEAR;
//	// Sampler address mode.
//	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//	// Anisotropy settings.
//	samplerInfo.anisotropyEnable = VK_TRUE;
//	samplerInfo.maxAnisotropy = 1;
//	// Border color
//	samplerInfo.unnormalizedCoordinates = VK_FALSE;
//	// Comparison functions disabled
//	samplerInfo.compareEnable = VK_FALSE;
//	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
//	// Mipmap mode
//	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//	samplerInfo.mipLodBias = 0.0f;
//	samplerInfo.minLod = 0.0f;
//	samplerInfo.maxLod = 0.0f;
//	// Create sampler
//	m_vkTextureSampler = new VkSampler();
//	if (vkCreateSampler(*m_vkEngineRef->vkGetDevice(), &samplerInfo,
//		nullptr, m_vkTextureSampler) != VK_SUCCESS)
//	{
//		throw std::runtime_error("Failed to create texture sampler!");
//	}
//}
//

#pragma endregion

#endif