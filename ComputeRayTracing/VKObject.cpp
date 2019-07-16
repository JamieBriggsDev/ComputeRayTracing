#if VK

#include "VKObject.h"
#include "VKPipeline.h"
#include "Camera.h"


VKObject::VKObject(VkDevice* _vkDevice, VkExtent2D _vkExtent, VkFormat _vkSwapChainFormat, 
	const char* _modelFilePath)
{
	m_model = new Model(_modelFilePath);

	m_pipeline = new VKPipeline(_vkDevice, _vkExtent, _vkSwapChainFormat, "Shaders/vert.spv",
		"Shaders/frag.spv");

	m_modelMatrix = glm::mat4();
}


VKObject::~VKObject()
{
}

void VKObject::Draw(Camera* _camera)
{

}

#endif